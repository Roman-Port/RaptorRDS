#include <raptorrds/client/client.h>
#include <stdio.h>
#include <malloc.h>
#include "settings.h"
#include <cassert>

char* rds_client::make_safe_string(const char* input, int len) {
	//Allocate space for this, plus the null terminator
	char* buffer = (char*)malloc(sizeof(char) * (len + 1));
	assert(buffer != 0);
	
	//Set null terminator
	buffer[len] = 0;

	//Convert
	char value;
	for (int i = 0; i < len; i++) {
		//Read
		value = input[i];

		//Change some special characters
		if (value == 0)
			value = ' ';
		if (value == '\r')
			value = '\\';
		if (value == '\n')
			value = '/';

		//If the value is a control character, replace with something else
		if (value < 32)
			value = '?';

		//It's now safe to log. Set
		buffer[i] = value;
	}

	return buffer;
}