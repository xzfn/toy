
#include "timer_manager.h"

#include <cassert>
#include <limits>
#include <stdexcept>


TimerManager::TimerManager()
{
}

void TimerManager::schedule(double current_time)
{
	m_current_time = current_time;
	while (!m_timer_queue.empty() 
		&& m_timer_queue.top()->timepoint <= m_current_time) {
		TimerData& timer_data = *m_timer_queue.top();
		m_timer_queue.pop();
		if (timer_data.cancelled) {
			m_timers.erase(timer_data.id);
			continue;
		}

		if (!timer_data.repeat) {
			timer_data.callback();
			m_timers.erase(timer_data.id);
		}
		else {
			timer_data.callback();
			timer_data.timepoint += timer_data.duration;
			m_timer_queue.push(&timer_data);
		}
	}
}


Timer TimerManager::add_timer(double duration, TimerCallback callback)
{
	Timer timer = generate_id();
	TimerData timer_data{
		timer,
		false,
		false,
		duration,
		m_current_time + duration,
		callback
	};
	add_timer_data(timer_data);
	return timer;
}

Timer TimerManager::add_repeat_timer(double duration, TimerCallback callback)
{
	if (duration <= 0.0) {
		throw std::runtime_error("TimerManager::add_repeat_timer duration should be greater than 0.0");
	}
	Timer timer = generate_id();
	TimerData timer_data{
		timer,
		true,
		false,
		duration,
		m_current_time + duration,
		callback
	};
	add_timer_data(timer_data);
	return timer;
}

void TimerManager::cancel_timer(Timer timer)
{
	auto it = m_timers.find(timer);
	if (it != m_timers.end()) {
		it->second.cancelled = true;
	}
}

void TimerManager::clear_timers()
{
	m_timers.clear();
	m_timer_queue.swap(decltype(m_timer_queue)());
}

Timer TimerManager::generate_id()
{
	m_incrementing_id += 1;
	return m_incrementing_id;
}

void TimerManager::add_timer_data(TimerData timer_data)
{
	Timer timer = timer_data.id;
	auto res = m_timers.insert(std::make_pair(timer, timer_data));
	auto it = res.first;
	auto success = res.second;
	assert(success);
	auto& timer_data_real = it->second;
	m_timer_queue.push(&timer_data_real);
}
