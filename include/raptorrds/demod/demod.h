#pragma once

#include <raptorrds/debug.h>
#include <raptorrds/demod/handler.h>
#include <raptordsp/filter/fir/filter_complex.h>
#include <raptordsp/filter/fir/filter_real.h>
#include <raptordsp/analog/costas_loop.h>
#include <raptordsp/analog/agc.h>
#include <raptordsp/digital/clock_recovery_mm_complex.h>
#include <raptorrds/debug.h>

#define RDS_SYMBOL_RATE 1187.5f

class rds_demod {

public:
	rds_demod(size_t buffer_size, rds_demod_handler* handler);
    void configure(float sampleRate);
	void process(float* ptr, int count);

private:
    rds_demod_handler* handler;

#ifdef RAPTORRDS_DEBUG_OUTPUT
    raptorrds_debug_sink debug_coarse;
    raptorrds_debug_sink debug_fine;
    raptorrds_debug_sink debug_loop;
    raptorrds_debug_sink debug_clock;
#endif

    raptor_complex* buffer;
    size_t buffer_size;
    raptor_complex ro_phase;
    raptor_complex ro_rotation;
    raptor_filter_ccc ro_filter_coarse;
    raptor_filter_ccc ro_filter_fine;
    raptor_agc_complex agc;
    raptor_costas_loop ro_loop;
    raptor_filter_ccf matched_filter;
    raptor_clock_recovery_mm_complex clock_recovery_mm;
    unsigned char dd_previous;
    unsigned char dd_discard;

    void process_differential_decoding(float sample);

};