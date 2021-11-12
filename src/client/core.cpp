#include <raptorrds/client/client.h>
#include <raptorrds/decode/message.h>


#include <cstring>
#include <stdio.h>
#include "settings.h"

rds_client::rds_client() {
	this->has_sync = false;
	this->picode = 0;
	this->pty = 0;
	reset();
}

void set_picode(rds_client* client, uint16_t picode) {
	//Ignore if it's already set to this
	if (client->picode == picode)
		return;

	//Send event
	rds_event_picode_update_t evt;
	evt.picode = picode;
	client->on_picode_update.broadcast(&evt);

	//Set
	client->picode = picode;

	//Log
#ifdef RDS_DEBUG_LOG
	printf("[RDS-SET-PI] %04X\n", picode);
#endif
}

void set_pty(rds_client* client, uint8_t pty) {
	//Ignore if it's already set to this
	if (client->pty == pty)
		return;

	//Send event
	rds_event_pty_update_t evt;
	evt.pty = pty;
	client->on_pty_update.broadcast(&evt);

	//Set
	client->pty = pty;

	//Log
#ifdef RDS_DEBUG_LOG
	printf("[RDS-SET-PTY] %04X\n", pty);
#endif
}

void print_frame(const char* label, rds_frame_t* frame) {
	char bin[100];
	int o = 0;
	for (int i = 0; i < 64; i++) {
		bin[o++] = RDS_READ_INT(frame, bool, i, 1) ? 'X' : '-';
		if (i % 16 == 0 && i != 0) {
			bin[o++] = ' ';
			bin[o++] = '|';
			bin[o++] = ' ';
		}
	}
	bin[o] = 0;
	printf("[%s] %s\n", label, bin);
}

void rds_client::push_frame(rds_frame_t frame) {
	//Log
#ifdef RDS_DEBUG_LOG_VERBOSE
	print_frame("RDS-FRAME", &frame);
#endif

	//Send event
	rds_event_frame_t evt;
	evt.frame = &frame;
	on_frame.broadcast(&evt);

	//Set common values
	set_picode(this, RDS_GET_PICODE(&frame));
	set_pty(this, RDS_GET_PTY(&frame));

	//Use message-specific items
	switch (rds_identify_frame(&frame)) {
	case rds_message_type_t::RDS_MESSAGE_BASIC_TUNING: process_basic_tuning(&frame); break;
	case rds_message_type_t::RDS_MESSAGE_RADIOTEXT: process_radiotext(&frame); break;
	}
}

void rds_client::sync_changed(bool sync) {
	this->has_sync = sync;
	rds_event_sync_changed_t evt;
	evt.sync = sync;
	on_sync_changed.broadcast(&evt);
}

void rds_client::reset() {
	set_picode(this, 0);
	set_pty(this, 0);

	memset(ps, 0, sizeof(ps));
	memset(ps_complete, 0, sizeof(ps_complete));
	ps_set_segments = 0;

	memset(rt, 0, sizeof(rt));
	memset(rt_complete, 0, sizeof(rt_complete));
	rt_ab = 0;
	rt_set_segments = 0;
	rt_completed = 0;
}