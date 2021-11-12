#pragma once

#include <raptorrds/decode/message.h>

#define RDS_RADIOTEXT_GET_AB(frame) RDS_READ_INT(frame, bool, RDS_OFFSET_GROUP_B_USER + 0, 1)
#define RDS_RADIOTEXT_GET_SEGMENTADDRESS(frame) RDS_READ_INT(frame, uint8_t, RDS_OFFSET_GROUP_B_USER + 1, 4)
#define RDS_RADIOTEXT_GET_CHARACTER(frame, index) RDS_READ_INT(frame, char, RDS_OFFSET_GROUP_C + ((index) * 8), 8)