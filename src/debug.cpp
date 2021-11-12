#ifdef RAPTORRDS_DEBUG_OUTPUT

#include <raptorrds/debug.h>

raptorrds_debug_sink::raptorrds_debug_sink() : file(0) {

}

raptorrds_debug_sink::~raptorrds_debug_sink() {
	if (file != 0)
		fclose(file);
	file = 0;
}

void raptorrds_debug_sink::open(const char* name, float sampleRate) {
	//Search for a path to open
	char fmt[512];
	int i = 1;
	do {
		sprintf(fmt, "RaptorRDS-Debug-%s-%i.bin", name, i);
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

void raptorrds_debug_sink::write(float* ptr, int count) {
	if (file != 0)
		fwrite(ptr, sizeof(float), count, file);
}

#endif