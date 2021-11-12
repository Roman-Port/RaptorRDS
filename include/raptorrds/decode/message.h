#pragma once

#include <stdint.h>
#include <raptorrds/sync/frame.h>

enum class rds_message_type_t {
	RDS_MESSAGE_UNKNOWN,
	RDS_MESSAGE_BASIC_TUNING,
	RDS_MESSAGE_RADIOTEXT
};

rds_message_type_t rds_identify_frame(rds_frame_t* frame);

#define RDS_OFFSET_GROUP_C 32
#define RDS_OFFSET_GROUP_D 48
#define RDS_OFFSET_END 64
#define RDS_OFFSET_GROUP_B_USER 27

#define RDS_GROUP_VERSION_A 0
#define RDS_GROUP_VERSION_B 1

#define RDS_READ_INT(frame, type, start, len) ((type)(((frame)->payload >> (64 - (len) - (start))) & (((uint64_t)1 << (len)) - 1)))

/* RDS Base type */

#define RDS_GET_PICODE(frame) RDS_READ_INT(frame, uint16_t, 0, 16)
#define RDS_GET_GROUP(frame) RDS_READ_INT(frame, uint8_t, 16, 5)
#define RDS_GET_GROUP_VERSION(frame) RDS_READ_INT(frame, bool, 20, 1)
#define RDS_GET_TRAFFIC(frame) RDS_READ_INT(frame, bool, 21, 1)
#define RDS_GET_PTY(frame) RDS_READ_INT(frame, uint8_t, 22, 5)