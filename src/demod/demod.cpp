#include <raptorrds/demod/demod.h>
#include <raptordsp/filter/builder/builder_lowpass.h>
#include <raptordsp/filter/builder/builder_bandpass.h>
#include <volk/volk.h>
#include <cassert>

#define RDS_SUBCARRIER_FREQ 57000
#define RDS_BW 2500

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

	//Open output
#ifdef RAPTORRDS_DEBUG_OUTPUT
	debug_coarse.open("DemodCoarse", sampleRate);
#endif

	//Configure fine decimation filter
	raptor_filter_builder_bandpass builderFine(sampleRate, -RDS_BW, RDS_BW);
	builderFine.automatic_tap_count(200, 80);
	ro_filter_fine.configure(builderFine.build_taps_complex(), builderFine.calculate_decimation(&sampleRate));

	//Open output
#ifdef RAPTORRDS_DEBUG_OUTPUT
	debug_fine.open("DemodFine", sampleRate);
#endif

	//Configure costas loop
	ro_loop.configure(0.1f, 2, false);

	//Open output
#ifdef RAPTORRDS_DEBUG_OUTPUT
	debug_loop.open("DemodCostasLoop", sampleRate);
#endif

	//Configure next
	configure_clock_recovery(sampleRate);
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

	//Debug write
#ifdef RAPTORRDS_DEBUG_OUTPUT
	debug_coarse.write((float*)buffer, count * 2);
#endif

	//Filter fine
	count = ro_filter_fine.process(buffer, buffer, count);

	//Debug write
#ifdef RAPTORRDS_DEBUG_OUTPUT
	debug_fine.write((float*)buffer, count * 2);
#endif

	//Process costas loop
	ro_loop.process(buffer, buffer, count);

	//Deinterlace back to a real signal
	float* bufferReal = (float*)buffer;
	for (int i = 0; i < count; i++)
		bufferReal[i] = buffer[i].real();

	//Debug write
#ifdef RAPTORRDS_DEBUG_OUTPUT
	debug_loop.write(bufferReal, count);
#endif

	//Send to next step
	process_clock_recovery(bufferReal, count);
}