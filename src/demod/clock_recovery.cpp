#include <raptorrds/demod/demod.h>
#include <cstring>
#include <cmath>
#include <raptordsp/filter/builder/builder_rootraised.h>
#include <volk/volk.h>
#include "interpolator_taps.h"

static float slice(float x) { return x < 0 ? -1.0F : 1.0F; }

static float branchless_clip(float x, float clip)
{
	float x1 = fabsf(x + clip);
	float x2 = fabsf(x - clip);
	x1 -= x2;
	return 0.5 * x1;
}

void rds_demod::configure_clock_recovery(float sampleRate) {
	//Configure the matched filter
	raptor_filter_builder_root_raised_cosine builder(sampleRate, RDS_SYMBOL_RATE * 2, 0.35f);
	builder.set_ntaps(361);
	matched_filter.configure(builder.build_taps_real(), 1);

	//Set up the clock recovery part
	d_mu = 0.5;
	d_gain_mu = 0.150f;
	d_gain_omega = 0.8f;
	d_omega_relative_limit = 0.001;
	d_omega = (sampleRate / (RDS_SYMBOL_RATE * 2));
	d_omega_mid = d_omega;
	d_omega_lim = d_omega_mid * d_omega_relative_limit;
	d_last_sample = 0;

	//Clear out and reset the buffer for clock recovery
	queue_use = 0;
	memset(queue, 0, sizeof(queue));

	//Configure next step
	configure_differential_decoding();
}

void rds_demod::process_clock_recovery(float* in, int count) {
	//Apply the matched filter
	count = matched_filter.process(in, in, count);

	//Process recovery
	float mm_val;
	float output;
	for (int ii = 0; ii < count; ii++) {
		if (interpolate(in[ii], &output)) {
			//Push to next step
			process_differential_decoding(output);

			//Process
			mm_val = slice(d_last_sample) * output - slice(output) * d_last_sample;
			d_last_sample = output;

			d_omega = d_omega + d_gain_omega * mm_val;
			d_omega = d_omega_mid + branchless_clip(d_omega - d_omega_mid, d_omega_lim);
			d_mu = d_mu + d_omega + d_gain_mu * mm_val;

			queue_use -= (int)floor(d_mu);
			d_mu = d_mu - floor(d_mu);
		}
	}
}

bool rds_demod::interpolate(float input, float* output) {
	//Push into queue
	memcpy(queue, &queue[1], sizeof(queue) - sizeof(float));
	queue[7] = input;
	queue_use++;

	//If the queue is not full, ignore
	if (queue_use < NTAPS) {
		return false;
	}

	//Get the index
	int step = (int)rint((1 - d_mu) * NSTEPS);

	//Apply interpolation
	volk_32f_x2_dot_prod_32f(output, queue, taps[step], NTAPS);

	return true;
}