#pragma once

#include <raptorrds/debug.h>
#include <raptorrds/demod/handler.h>
#include <raptordsp/filter/fir/filter_complex.h>
#include <raptordsp/filter/fir/filter_real.h>
#include <raptordsp/analog/costas_loop.h>

#define RDS_SYMBOL_RATE 1187.5f

class rds_demod {

public:
	rds_demod(size_t buffer_size, rds_demod_handler* handler);
    void configure(float sampleRate);
	void process(float* ptr, int count);

private:
    rds_demod_handler* handler;

#ifdef RAPTORRDS_DEBUG_OUTPUT
#include <raptorrds/debug.h>
    raptorrds_debug_sink debug_coarse;
    raptorrds_debug_sink debug_fine;
    raptorrds_debug_sink debug_loop;
#endif

    /* ROTATION */

    raptor_complex* buffer;
    size_t buffer_size;
    raptor_complex ro_phase;
    raptor_complex ro_rotation;
    raptor_filter_ccc ro_filter_coarse;
    raptor_filter_ccc ro_filter_fine;
    raptor_costas_loop ro_loop;

    /* CLOCK RECOVERY */

    void configure_clock_recovery(float rdsSampleRate);
    void process_clock_recovery(float* ptr, int count);
    bool interpolate(float input, float* output);

    raptor_filter_real matched_filter;

    float queue[8];
    int queue_use;
    
    float d_mu;
    float d_gain_mu;
    float d_omega;
    float d_gain_omega;
    float d_omega_relative_limit;
    float d_omega_mid;
    float d_omega_lim;
    float d_last_sample;

    /* DIFFERENTIAL DECODING */

    void configure_differential_decoding();
    void process_differential_decoding(float sample);

    unsigned char dd_previous;
    unsigned char dd_discard;
    

};