#pragma once

class rds_demod_handler {

public:
	virtual void rds_demod_process(unsigned char bit) = 0;

};