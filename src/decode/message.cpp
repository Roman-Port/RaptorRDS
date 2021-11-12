#include <raptorrds/decode/message.h>

rds_message_type_t rds_identify_frame(rds_frame_t* frame) {
	switch (RDS_GET_GROUP(frame)) {
	case 0b00000: return rds_message_type_t::RDS_MESSAGE_BASIC_TUNING;
	case 0b00001: return rds_message_type_t::RDS_MESSAGE_BASIC_TUNING;
	case 0b00100: return rds_message_type_t::RDS_MESSAGE_RADIOTEXT;
	case 0b00101: return rds_message_type_t::RDS_MESSAGE_RADIOTEXT;
	}
	return rds_message_type_t::RDS_MESSAGE_UNKNOWN;
}