#pragma once

#include <raptorrds/sync/frame.h>

class rds_sync_handler {

public:
	virtual void sync_changed(bool sync) = 0;
	virtual void push_frame(rds_frame_t frame) = 0;

};