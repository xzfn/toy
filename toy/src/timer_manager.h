#pragma once

// A timer manager, driven by external tick with passed in time.
//
// Notes and Guidelines:
// Externally driven;
// Minimize dependency;
// Anything can happen in a callback,
// So be careful with container operations;


#include <cstdint>

#include <functional>
#include <queue>
#include <map>


typedef uint64_t Timer;
typedef std::function<void(void)> TimerCallback;

struct TimerData {
	Timer id;
	bool repeat;
	bool cancelled;
	double duration;
	double timepoint;
	TimerCallback callback;
};

struct TickTimerData {
	Timer id;
	bool cancelled;
	TimerCallback callback;
};

struct TimerDataCompare {
	bool operator()(TimerData* data1, TimerData* data2) {
		return data1->timepoint > data2->timepoint;
	}
};


class TimerManager {

public:
	TimerManager();

	TimerManager(const TimerManager&) = delete;
	TimerManager& operator=(const TimerManager&) = delete;

	void schedule(double current_time);

	Timer add_timer(double duration, TimerCallback cb);
	Timer add_repeat_timer(double duration, TimerCallback callback);
	Timer add_tick_timer(TimerCallback callback);

	void cancel_timer(Timer timer);
	void cancel_tick_timer(Timer timer);

	void clear_timers();

private:
	Timer generate_id();
	void add_timer_data(TimerData timer_data);
	void add_tick_timer_data(TickTimerData timer_data);

private:
	// NOTE MUST use std::map, not std::unordered_map
	std::map<Timer, TimerData> m_timers;
	std::priority_queue<TimerData*, std::vector<TimerData*>, TimerDataCompare> m_timer_queue;
	std::map<Timer, TimerData> m_pending_add_timers;

	std::map<Timer, TickTimerData> m_tick_timers;
	std::map<Timer, TickTimerData> m_pending_add_tick_timers;

	double m_current_time = 0.0;
	uint64_t m_incrementing_id = 0;
	bool m_is_scheduling = false;
};
