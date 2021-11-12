#pragma once

struct rds_data_pty_t {

	const char* name;
	const char* name_short;

};

struct rds_data_t {

	const rds_data_pty_t pty_table[32];

};

extern rds_data_t RDS_DATA_US;
extern rds_data_t RDS_DATA_EUROPE;