#pragma once

#include <raptorrds/demod/demod.h>
#include <raptorrds/sync/sync.h>
#include <raptorrds/client/client.h>

class rds_pipeline : public rds_client {

public:
	rds_pipeline(size_t buffer_size);
	void configure(float sampleRate);
	void process(float* ptr, int count);

private:
	rds_sync sync;
	rds_demod demod;

};