#pragma once

#ifdef RAPTORRDS_DEBUG_OUTPUT

#include <stdio.h>

class raptorrds_debug_sink {

public:
	raptorrds_debug_sink();
	~raptorrds_debug_sink();
	void open(const char* name, float sampleRate);
	void write(float* ptr, int count);

private:
	FILE* file;

};

#endif