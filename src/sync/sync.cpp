#include <raptorrds/sync/sync.h>

// much of this file is borrowed from https://github.com/bastibl/gr-rds/blob/maint-3.9/lib/decoder_impl.cc

static const unsigned int offset_pos[5] = { 0,1,2,3,2 };
static const unsigned int offset_word[5] = { 252,408,360,436,848 };
static const unsigned int syndrome[5] = { 383,14,303,663,748 };
static const char* const offset_name[] = { "A","B","C","D","C'" };

rds_sync::rds_sync(rds_sync_handler* handler) {
	this->handler = handler;
	reset();
}

unsigned int calc_syndrome(unsigned long message, unsigned char mlen) {
	unsigned long reg = 0;
	unsigned int i;
	const unsigned long poly = 0x5B9;
	const unsigned char plen = 10;

	for (i = mlen; i > 0; i--) {
		reg = (reg << 1) | ((message >> (i - 1)) & 0x01);
		if (reg & (1 << plen)) reg = reg ^ poly;
	}
	for (i = plen; i > 0; i--) {
		reg = reg << 1;
		if (reg & (1 << plen)) reg = reg ^ poly;
	}
	return (reg & ((1 << plen) - 1));	// select the bottom plen bits of reg
}

void rds_sync::process(unsigned char* input, int count) {
	unsigned long bit_distance, block_distance;
	unsigned int block_calculated_crc, block_received_crc, checkword, dataword;
	unsigned int reg_syndrome;
	unsigned char offset_char('x');  // x = error while decoding the word offset

/* the synchronization process is described in Annex C, page 66 of the standard */
	for (int i = 0; i < count; i++) {
		reg = (reg << 1) | input[i];		// reg contains the last 26 rds bits
		switch (d_state) {
			case NO_SYNC:
				reg_syndrome = calc_syndrome(reg, 26);
				for (int j = 0; j < 5; j++) {
					if (reg_syndrome == syndrome[j]) {
						if (!presync) {
							lastseen_offset = j;
							lastseen_offset_counter = bit_counter;
							presync = true;
						}
						else {
							bit_distance = bit_counter - lastseen_offset_counter;
							if (offset_pos[lastseen_offset] >= offset_pos[j])
								block_distance = offset_pos[j] + 4 - offset_pos[lastseen_offset];
							else
								block_distance = offset_pos[j] - offset_pos[lastseen_offset];
							if ((block_distance * 26) != bit_distance) presync = false;
							else
								enter_sync(j);
						}
						break; //syndrome found, no more cycles
					}
				}
				break;
			case SYNC:
				/* wait until 26 bits enter the buffer */
				if (block_bit_counter < 25) block_bit_counter++;
				else {
					good_block = false;
					dataword = (reg >> 10) & 0xffff;
					block_calculated_crc = calc_syndrome(dataword, 16);
					checkword = reg & 0x3ff;
					/* manage special case of C or C' offset word */
					if (block_number == 2) {
						block_received_crc = checkword ^ offset_word[block_number];
						if (block_received_crc == block_calculated_crc) {
							good_block = true;
							offset_char = 'C';
						}
						else {
							block_received_crc = checkword ^ offset_word[4];
							if (block_received_crc == block_calculated_crc) {
								good_block = true;
								offset_char = 'c';  // C' (C-Tag)
							}
							else {
								wrong_blocks_counter++;
								good_block = false;
							}
						}
					}
					else {
						block_received_crc = checkword ^ offset_word[block_number];
						if (block_received_crc == block_calculated_crc) {
							good_block = true;
							if (block_number == 0) offset_char = 'A';
							else if (block_number == 1) offset_char = 'B';
							else if (block_number == 3) offset_char = 'D';
						}
						else {
							wrong_blocks_counter++;
							good_block = false;
						}
					}
					/* done checking CRC */
					if (block_number == 0 && good_block) {
						group_assembly_started = true;
						group_good_blocks_counter = 1;
					}
					if (group_assembly_started) {
						if (!good_block) group_assembly_started = false;
						else {
							group[block_number] = dataword;
							offset_chars[block_number] = offset_char;
							group_good_blocks_counter++;
						}
						if (group_good_blocks_counter == 5) export_frame(group);
					}
					block_bit_counter = 0;
					block_number = (block_number + 1) % 4;
					blocks_counter++;
					/* 1187.5 bps / 104 bits = 11.4 groups/sec, or 45.7 blocks/sec */
					if (blocks_counter == 50) {
						if (wrong_blocks_counter > 35)
							exit_sync();
						blocks_counter = 0;
						wrong_blocks_counter = 0;
					}
				}
				break;
			default:
				d_state = NO_SYNC;
				break;
		}
		i++;
		bit_counter++;
	}
}

void rds_sync::enter_sync(unsigned int sync_block_number) {
	wrong_blocks_counter = 0;
	blocks_counter = 0;
	block_bit_counter = 0;
	block_number = (sync_block_number + 1) % 4;
	group_assembly_started = false;
	d_state = SYNC;
	handler->sync_changed(true);
}

void rds_sync::exit_sync() {
	reset();
	handler->sync_changed(false);
}

void rds_sync::reset() {
	presync = false;
	d_state = NO_SYNC;
}

void rds_sync::export_frame(unsigned int* group) {
	//Pack into frame
	rds_frame_t frame;

	//Transfer payload
	unsigned char* bytes = (unsigned char*)&frame.payload;
	bytes[7] = (group[0] >> 8U) & 0xffU;
	bytes[6] = (group[0]) & 0xffU;
	bytes[5] = (group[1] >> 8U) & 0xffU;
	bytes[4] = (group[1]) & 0xffU;
	bytes[3] = (group[2] >> 8U) & 0xffU;
	bytes[2] = (group[2]) & 0xffU;
	bytes[1] = (group[3] >> 8U) & 0xffU;
	bytes[0] = (group[3]) & 0xffU;


	//Transfer offset words
	frame.offsets[0] = offset_chars[0];
	frame.offsets[1] = offset_chars[1];
	frame.offsets[2] = offset_chars[2];
	frame.offsets[3] = offset_chars[3];

	//Dispatch
	handler->push_frame(frame);
}