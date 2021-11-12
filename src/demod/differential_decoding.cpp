#include <raptorrds/demod/demod.h>

void rds_demod::configure_differential_decoding() {
	dd_discard = false;
	dd_previous = 0;
}

void rds_demod::process_differential_decoding(float sample) {
	//Discard every other sample
	if (dd_discard) {
		dd_discard = false;
		return;
	}

	//Binary slice
	unsigned char value = sample > 0 ? 1 : 0;

	//Do differential decoding on output
	unsigned char temp = (value ^ dd_previous) & 1;
	dd_previous = value;

	//Push to output
	handler->rds_demod_process(temp);

	//Update state
	dd_discard = true;
}