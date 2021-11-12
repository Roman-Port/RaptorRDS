#include <raptorrds/pipeline.h>

rds_pipeline::rds_pipeline(size_t buffer_size) : sync(this), demod(buffer_size, &sync) {

}

void rds_pipeline::configure(float sampleRate) {
	demod.configure(sampleRate);
}

void rds_pipeline::process(float* ptr, int count) {
	demod.process(ptr, count);
}