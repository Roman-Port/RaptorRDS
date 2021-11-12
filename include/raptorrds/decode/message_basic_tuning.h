#pragma once

#include <raptorrds/decode/message.h>

#define RDS_BASICTUNING_GET_TRAFFICANNOUNCEMENT(frame)		RDS_READ_INT(frame, bool,	 RDS_OFFSET_GROUP_B_USER + 0, 1)
#define RDS_BASICTUNING_GET_MUSIC(frame)					RDS_READ_INT(frame, bool,	 RDS_OFFSET_GROUP_B_USER + 1, 1)
#define RDS_BASICTUNING_GET_DI(frame)						RDS_READ_INT(frame, bool,	 RDS_OFFSET_GROUP_B_USER + 2, 1)
#define RDS_BASICTUNING_GET_SEGMENT_ADDRESS(frame)			RDS_READ_INT(frame, uint8_t, RDS_OFFSET_GROUP_B_USER + 3, 2)
#define RDS_BASICTUNING_GET_CHAR_A(frame)					RDS_READ_INT(frame, char,    RDS_OFFSET_GROUP_D + 0,      8)
#define RDS_BASICTUNING_GET_CHAR_B(frame)					RDS_READ_INT(frame, char,    RDS_OFFSET_GROUP_D + 8,      8)