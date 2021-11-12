#pragma once

#include <raptordsp/demodulator/demod_bcast_fm_stereo.h>
#include <raptorrds/pipeline.h>

class demod_bcast_fm_stereo_rds : public demod_bcast_fm_stereo {

public:
	demod_bcast_fm_stereo_rds(size_t buffer_size, float deemphasisTime = DEEMPHASIS_TIME_USA);
	virtual float configure(float sampleRate) override;
	bool is_rds_detected();
	rds_client* get_rds();

protected:
	virtual void audio_filtered(float* audioL, float* audioR, int mpxCount, int audioCount) override;

private:
	rds_pipeline rds;


};