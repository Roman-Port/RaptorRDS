#include <raptorrds/demod/demod.h>
#include <raptordsp/filter/builder/builder_lowpass.h>
#include <raptordsp/filter/builder/builder_bandpass.h>
#include <raptordsp/filter/builder/builder_rootraised.h>
#include <volk/volk.h>
#include <cassert>

#define RDS_SUBCARRIER_FREQ 57000
#define RDS_BW 2500

#ifdef RAPTORRDS_DEBUG_OUTPUT
#define DEBUG_OPEN(name, sampleRate) name.open(#name, sampleRate);
#define DEBUG_WRITE(name, buffer, count) name.write(buffer, count);
#else
#define DEBUG_OPEN(name, sampleRate)
#define DEBUG_WRITE(name, buffer, count)
#endif

rds_demod::rds_demod(size_t buffer_size, rds_demod_handler* handler) : handler(handler), buffer_size(buffer_size) {
	buffer = (raptor_complex*)malloc(sizeof(raptor_complex) * buffer_size);
	assert(buffer != 0);
}

void rds_demod::configure(float sampleRate) {
	//Configure rotator
	float freq = 2.0f * M_PI * RDS_SUBCARRIER_FREQ / sampleRate;
	ro_rotation = raptor_complex(std::cos(freq), std::sin(freq));
	ro_phase = raptor_complex(1, 0);

	//Configure coarse decimation filter
	raptor_filter_builder_bandpass builderCoarse(sampleRate, -6000, 6000);
	builderCoarse.automatic_tap_count(3000, 80);
	ro_filter_coarse.configure(builderCoarse.build_taps_complex(), builderCoarse.calculate_decimation(&sampleRate));
	DEBUG_OPEN(debug_coarse, sampleRate)

	//Configure fine decimation filter
	raptor_filter_builder_bandpass builderFine(sampleRate, -RDS_BW, RDS_BW);
	builderFine.automatic_tap_count(200, 80);
	ro_filter_fine.configure(builderFine.build_taps_complex(), builderFine.calculate_decimation(&sampleRate));
	DEBUG_OPEN(debug_fine, sampleRate)

	//Configure costas loop
	//ro_loop.configure((2 * M_PI) / 100, 2, false);
	ro_loop.configure(M_PI / 100, 2, false);
	
	DEBUG_OPEN(debug_loop, sampleRate)

	//Configure the matched filter
	raptor_filter_builder_root_raised_cosine builder(sampleRate, RDS_SYMBOL_RATE * 2, 0.35f);
	builder.set_ntaps(361);
	matched_filter.configure(builder.build_taps_real(), 1);

	//Configure the clock recovery
	clock_recovery_mm.configure(
		(sampleRate / (RDS_SYMBOL_RATE * 2)),
		0.8f,
		0.5f,
		0.150f,
		0.001f
	);
	DEBUG_OPEN(debug_clock, RDS_SYMBOL_RATE * 2)

	//Reset differential decoding
	dd_discard = false;
	dd_previous = 0;
}

void rds_demod::process(float* in, int count) {
	//Copy real input in
	assert(count <= buffer_size);
	for (int i = 0; i < count; i++)
		buffer[i] = raptor_complex(in[i], 0);

	//Rotate all
	volk_32fc_s32fc_x2_rotator_32fc(buffer, buffer, ro_rotation, &ro_phase, count);

	//Apply coarse
	count = ro_filter_coarse.process(buffer, buffer, count);
	DEBUG_WRITE(debug_coarse, buffer, count)

	//Filter fine
	count = ro_filter_fine.process(buffer, buffer, count);
	DEBUG_WRITE(debug_fine, buffer, count)

	//Process AGC
	agc.process(buffer, buffer, count);

	//Process costas loop
	ro_loop.process(buffer, buffer, count);
	DEBUG_WRITE(debug_loop, buffer, count)

	//Apply the matched filter
	count = matched_filter.process(buffer, buffer, count);

	//Process clock recovery
	count = clock_recovery_mm.process(buffer, count, buffer, buffer_size);
	DEBUG_WRITE(debug_clock, buffer, count)

	//Process each sample
	for (int i = 0; i < count; i++) {
		//Get only real part and process the differential decoding on it
		process_differential_decoding(buffer[i].real());
	}
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