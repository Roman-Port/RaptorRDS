#ifdef RAPTORRDS_DEBUG_OUTPUT

#include <raptorrds/debug.h>
#include <cmath>

raptorrds_debug_sink::raptorrds_debug_sink() : file(0) {

}

raptorrds_debug_sink::raptorrds_debug_sink(const raptorrds_debug_sink& src) :
	file(0)
{

}

raptorrds_debug_sink::~raptorrds_debug_sink() {
	if (file != 0)
		fclose(file);
	file = 0;
}

void raptorrds_debug_sink::open(const char* name, float sampleRate) {
	//Search for a path to open
	char fmt[1024];
	int i = 1;
	do {
		sprintf(fmt, "RaptorRDS-Debug-%s-%i-%i.bin", name, (int)std::round(sampleRate), i);
		file = fopen(fmt, "wb");
		i++;
		if (i == 100) {
			printf("[RaptorRDS-Debug-%s] Failed to open debug output file.\n", name);
			break;
		}
	} while (file == 0);

	//Log
	if (file != 0)
		printf("[RaptorRDS-Debug-%s] Opened output at %f samples/sec on %s.\n", name, sampleRate, fmt);
}

void raptorrds_debug_sink::write(raptor_complex* ptr, int count) {
	write((float*)ptr, count * 2);
}

void raptorrds_debug_sink::write(float* ptr, int count) {
	if (file != 0)
		fwrite(ptr, sizeof(float), count, file);
}

#endif