#include <raptorrds/demod_bcast_fm_stereo_rds.h>

demod_bcast_fm_stereo_rds::demod_bcast_fm_stereo_rds(size_t buffer_size, float deemphasisTime) : demod_bcast_fm_stereo(buffer_size, deemphasisTime), rds(buffer_size) {

}

float demod_bcast_fm_stereo_rds::configure(float sampleRate) {
	rds.configure(sampleRate);
	return demod_bcast_fm_stereo::configure(sampleRate);
}

bool demod_bcast_fm_stereo_rds::is_rds_detected() {
	return rds.has_sync;
}

rds_client* demod_bcast_fm_stereo_rds::get_rds() {
	return &rds;
}

void demod_bcast_fm_stereo_rds::audio_filtered(float* audioL, float* audioR, int mpxCount, int audioCount) {
	demod_bcast_fm_stereo::audio_filtered(audioL, audioR, mpxCount, audioCount);

	//Demodulate RDS. NOTE THAT THIS WILL NUKE THE MPX BUFFER!
	rds.process(mpx_buffer, mpxCount);
}