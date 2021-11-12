#pragma once

#include <stdint.h>

struct rds_frame_t {

	uint64_t payload;
	unsigned char offsets[4];

};