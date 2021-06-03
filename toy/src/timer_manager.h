#pragma once

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
	void cancel_timer(Timer timer);
	void clear_timers();

private:
	Timer generate_id();
	void add_timer_data(TimerData timer_data);

private:
	std::map<Timer, TimerData> m_timers;
	std::priority_queue<TimerData*, std::vector<TimerData*>, TimerDataCompare> m_timer_queue;
	double m_current_time = 0.0;
	uint64_t m_incrementing_id = 0;
};
