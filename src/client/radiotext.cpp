#include <raptorrds/client/client.h>
#include <raptorrds/decode/message_radiotext.h>
#include <stdio.h>
#include <malloc.h>
#include <cstring>
#include "settings.h"

bool is_message_complete(char* rt, uint64_t set_chars, uint64_t max_chars) {
	//Declare it to start steps
	bool complete = false;

	//First, check if all blocks are used
	complete = complete || set_chars == max_chars;

	//This is also defined as having a carraige return (ASCII 13). Look for that, but stop if we hit a gap
	for (int i = 0; i < RDS_RT_LEN && ((set_chars >> i) & 1); i++)
		complete = complete || rt[i] == 13;

	return complete;
}

void rds_client::process_radiotext(rds_frame_t* frame) {
	//Read data from the frame
	bool extended = RDS_GET_GROUP_VERSION(frame) == RDS_GROUP_VERSION_A; //Extended (A) has 4 characters each, non-extended (B) has 2
	bool ab = RDS_RADIOTEXT_GET_AB(frame);
	int segAddress = RDS_RADIOTEXT_GET_SEGMENTADDRESS(frame);

	//Clear if needed
	if (ab != rt_ab) {
		rt_clear();
		rt_ab = ab;
	}

	//Read out characters
	int segLen = (extended ? 4 : 2);
	int segSrc = (extended ? 0 : 2);
	char buffer[4];
	for (int i = 0; i < segLen; i++)
		buffer[i] = RDS_RADIOTEXT_GET_CHARACTER(frame, segSrc + i);

	//Convert the segment address to the real address, then copy
	int addr = segAddress * segLen;
	memcpy(&rt[addr], buffer, segLen * sizeof(char));

	//Set flag for all characters we just set
	rt_set_segments |= (((uint64_t)segLen * segLen) - 1) << addr;

	//Send event
	{
		rds_event_rt_partial_update_t evt;
		evt.rt = rt;
		evt.address = addr;
		evt.updated = segLen;
		on_rt_partial_update.broadcast(&evt);
	}

	//We'll now detect the message is complete. This is defined as either using all blocks or having a carraige return (13)
	bool complete = !rt_completed && is_message_complete(rt, rt_set_segments, extended ? 0xFFFFFFFFFFFFFFFF : 0xFFFFFFFF);

	//If it is complete, apply
	if (complete) {
		//Transfer over
		memcpy(rt_complete, rt, sizeof(rt));

		//Flag as completed so we don't resend the events
		rt_completed = 1;

		//Send event
		rds_event_rt_complete_update_t evt;
		evt.rt = ps_complete;
		on_rt_complete_update.broadcast(&evt);
	}

	//Log
#ifdef RDS_DEBUG_LOG
	char* safeSrc = rds_client_make_logging_safe_string(buffer, segLen);
	char* safeRt = rds_client_make_logging_safe_string(rt, RDS_RT_LEN);
	printf("[RDS-SET-RT] (%s) @ %02i -> [%s] %s\n", safeSrc, addr, safeRt, complete ? "COMPLETE" : "");
	free(safeSrc);
	free(safeRt);
#endif
}

void rds_client::rt_clear() {
	//Clear
	memset(rt, 0, sizeof(rt));
	memset(rt_complete, 0, sizeof(rt_complete));
	rt_ab = 0;
	rt_set_segments = 0;
	rt_completed = 0;

	//Send events
	{
		rds_event_rt_cleared_t evt;
		evt.ab = 0;
		on_rt_cleared.broadcast(&evt);
	}
	{
		rds_event_rt_partial_update_t evt;
		evt.address = 0;
		evt.rt = rt;
		evt.updated = 0;
		on_rt_partial_update.broadcast(&evt);
	}
	{
		rds_event_rt_complete_update_t evt;
		evt.rt = rt;
		on_rt_complete_update.broadcast(&evt);
	}

	//Log
#ifdef RDS_DEBUG_LOG
	printf("[RDS-SET-RT] CLEARED (flag %c)\n", ab ? 'A' : 'B');
#endif
}