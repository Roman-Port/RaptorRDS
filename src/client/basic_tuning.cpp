#include <raptorrds/client/client.h>
#include <raptorrds/decode/message_basic_tuning.h>
#include <stdio.h>
#include <malloc.h>
#include <cstring>
#include "settings.h"

void rds_client::process_basic_tuning(rds_frame_t* frame) {
	//Read info
	int segAddress = RDS_BASICTUNING_GET_SEGMENT_ADDRESS(frame);
	char a = RDS_BASICTUNING_GET_CHAR_A(frame);
	char b = RDS_BASICTUNING_GET_CHAR_B(frame);

	//Convert the segment address to the real address
	int addr = segAddress * 2;

	//Apply
	ps_set_segments |= 3 << addr;
	ps[addr + 0] = a;
	ps[addr + 1] = b;

	//Send event
	{
		rds_event_ps_partial_update_t evt;
		evt.ps = ps;
		evt.address = addr;
		on_ps_partial_update.broadcast(&evt);
	}

	//Check if this is complete (all segments set and this is the last segment)
	bool complete = ps_set_segments == 255 && addr == 6;
	if (complete) {
		//Transfer over
		memcpy(ps_complete, ps, sizeof(ps));

		//Reset completion
		ps_set_segments = 0;

		//Send event
		rds_event_ps_complete_update_t evt;
		evt.ps = ps_complete;
		on_ps_complete_update.broadcast(&evt);
	}

	//Log
#ifdef RDS_DEBUG_LOG
	char* safe = rds_client_make_logging_safe_string(ps, RDS_PS_LEN);
	printf("[RDS-SET-PS] (%c%c) @ %i -> [%s] %s\n", ps[addr + 0], ps[addr + 1], addr, safe, complete ? "COMPLETE" : "");
	free(safe);
#endif
}