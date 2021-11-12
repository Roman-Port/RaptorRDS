#pragma once

#include <stdint.h>
#include <raptorrds/sync/handler.h>
#include <raptorrds/sync/frame.h>

#define RDS_PS_LEN 8
#define RDS_RT_LEN 64

#define RDS_EVENT_DISPATCHER_MAX_CLIENTS 4
#define DECLARE_RDS_EVENT(name, ...) \
	struct rds_event_##name##_t { __VA_ARGS__ }; \
	class rds_event_##name##_handler { public: virtual void handle_rds_##name##(rds_event_##name##_t* evt) = 0;}; \
	class rds_event_##name##_dispatcher { \
	public:\
		rds_event_##name##_dispatcher() {for (int i = 0; i < RDS_EVENT_DISPATCHER_MAX_CLIENTS; i++) { bound[i] = 0; }} \
		void broadcast(rds_event_##name##_t* evt) { for (int i = 0; i < RDS_EVENT_DISPATCHER_MAX_CLIENTS; i++) { if(bound[i] != 0) { bound[i]->handle_rds_##name##(evt); } } } \
		bool bind(rds_event_##name##_handler* handler) { for (int i = 0; i < RDS_EVENT_DISPATCHER_MAX_CLIENTS; i++) { if(bound[i] == 0) { bound[i] = handler; return true; } } return false; } \
		bool unbind(rds_event_##name##_handler* handler) { bool found = false; for (int i = 0; i < RDS_EVENT_DISPATCHER_MAX_CLIENTS; i++) { if(bound[i] == handler) { bound[i] = 0; found = true; } } return found; } \
	private:\
		rds_event_##name##_handler* bound[RDS_EVENT_DISPATCHER_MAX_CLIENTS];\
	\
	};

DECLARE_RDS_EVENT(frame, rds_frame_t* frame;)
DECLARE_RDS_EVENT(sync_changed, bool sync;)
DECLARE_RDS_EVENT(picode_update, uint16_t picode;)
DECLARE_RDS_EVENT(pty_update, uint8_t pty;)
DECLARE_RDS_EVENT(ps_partial_update, const char* ps; int address;)
DECLARE_RDS_EVENT(ps_complete_update, const char* ps;)
DECLARE_RDS_EVENT(rt_cleared, bool ab;)
DECLARE_RDS_EVENT(rt_partial_update, const char* rt; int address; int updated;)
DECLARE_RDS_EVENT(rt_complete_update, const char* rt;)

class rds_client : public rds_sync_handler {

public:
	rds_client();
	virtual void sync_changed(bool sync) override;
	virtual void push_frame(rds_frame_t frame) override;
	void reset();

	uint16_t picode;
	uint8_t pty;
	bool has_sync;

	rds_event_frame_dispatcher on_frame;
	rds_event_sync_changed_dispatcher on_sync_changed;
	rds_event_picode_update_dispatcher on_picode_update;
	rds_event_pty_update_dispatcher on_pty_update;

	/* PROGRAM SERVICE */
public:
	char ps[RDS_PS_LEN];
	char ps_complete[RDS_PS_LEN];
	rds_event_ps_partial_update_dispatcher on_ps_partial_update;
	rds_event_ps_complete_update_dispatcher on_ps_complete_update;
private:
	uint8_t ps_set_segments;

	/* RADIO TEXT */
public:
	char rt[RDS_RT_LEN];
	char rt_complete[RDS_RT_LEN];
private:
	rds_event_rt_cleared_dispatcher on_rt_cleared;
	rds_event_rt_partial_update_dispatcher on_rt_partial_update;
	rds_event_rt_complete_update_dispatcher on_rt_complete_update;
	bool rt_ab;
	bool rt_completed;
	uint64_t rt_set_segments;

private:
	void process_basic_tuning(rds_frame_t* frame);
	void process_radiotext(rds_frame_t* frame);

	void rt_clear(bool ab);

};

char* rds_client_make_logging_safe_string(const char* input, int len);