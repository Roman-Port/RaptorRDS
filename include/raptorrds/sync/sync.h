#pragma once

#include <raptorrds/sync/handler.h>
#include <raptorrds/demod/handler.h>

class rds_sync : public rds_demod_handler {

public:
	rds_sync(rds_sync_handler* handler);
	void process(unsigned char* input, int count);
	void reset();

protected:
	virtual void rds_demod_process(unsigned char bit) override { process(&bit, 1); }

private:
	unsigned long  bit_counter;
	unsigned long  lastseen_offset_counter, reg;
	unsigned int   block_bit_counter;
	unsigned int   wrong_blocks_counter;
	unsigned int   blocks_counter;
	unsigned int   group_good_blocks_counter;
	unsigned int   group[4];
	unsigned char  offset_chars[4];  // [ABCcDEx] (x=error)
	bool           log;
	bool           debug;
	bool           presync;
	bool           good_block;
	bool           group_assembly_started;
	unsigned char  lastseen_offset;
	unsigned char  block_number;
	enum { NO_SYNC, SYNC } d_state;

	rds_sync_handler* handler;

	void enter_sync(unsigned int sync_block_number);
	void exit_sync();
	void export_frame(unsigned int* group);

};