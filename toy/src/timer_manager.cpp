
#include "timer_manager.h"

#include <cassert>
#include <limits>
#include <stdexcept>

#include "scope_util.h"


// whether add to m_timers and m_timer_queue immediately (false)
// or to m_pending_add_timers first if duration schedule (true).
// if false, zero duration timer will be triggered as soon as possible.
// if true, zero duration timer will be triggered next frame.
static constexpr bool g_use_pending_add = true;


TimerManager::TimerManager()
{
}

void TimerManager::schedule(double current_time)
{
	scopeutil::ScopedTrueToFalse scoped_scheduling(m_is_scheduling);

	m_current_time = current_time;

	// tick timers, trigger at every schedule
	for (auto it = m_tick_timers.begin(); it != m_tick_timers.end(); /*no inc*/) {
		TickTimerData& timer_data = it->second;
		if (timer_data.cancelled) {
			it = m_tick_timers.erase(it);
		}
		else {
			++it;
			timer_data.callback();
		}
	}

	// regular timers
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
			// NOTE Seems not a good idea if duration is very small.
			// may have to trigger multiple times in one schedule to keep up.
			timer_data.timepoint += timer_data.duration;
			m_timer_queue.push(&timer_data);
		}
	}

	// add pending timers
	if (!m_pending_add_timers.empty()) {
		for (auto& kv : m_pending_add_timers) {
			add_timer_data(kv.second);
		}
		m_pending_add_timers.clear();
	}

	// add pending tick timers
	if (!m_pending_add_tick_timers.empty()) {
		for (auto& kv : m_pending_add_tick_timers) {
			m_tick_timers.insert(kv);
		}
		m_pending_add_tick_timers.clear();
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
	if (m_is_scheduling && g_use_pending_add) {
		m_pending_add_timers.insert(std::make_pair(timer, timer_data));
	}
	else {
		add_timer_data(timer_data);
	}
	return timer;
}

Timer TimerManager::add_repeat_timer(double duration, TimerCallback callback)
{
	if (duration <= 0.0) {
		return add_tick_timer(callback);
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
	if (m_is_scheduling && g_use_pending_add) {
		m_pending_add_timers.insert(std::make_pair(timer, timer_data));
	}
	else {
		add_timer_data(timer_data);
	}
	return timer;
}

Timer TimerManager::add_tick_timer(TimerCallback callback)
{
	Timer timer = generate_id();
	TickTimerData timer_data{
		timer,
		false,
		callback
	};
	if (m_is_scheduling) {  // always use pending when scheduling seems better for tick timer
		add_tick_timer_data(timer_data);
	}
	else {
		m_tick_timers.insert(std::make_pair(timer, timer_data));
	}
	return timer;
}

void TimerManager::cancel_timer(Timer timer)
{
	auto it = m_timers.find(timer);
	if (it != m_timers.end()) {
		it->second.cancelled = true;
		return;
	}

	auto it_pending = m_pending_add_timers.find(timer);
	if (it_pending != m_pending_add_timers.end()) {
		it_pending->second.cancelled = true;
		return;
	}

	cancel_tick_timer(timer);
}

void TimerManager::cancel_tick_timer(Timer timer)
{
	auto it_tick = m_tick_timers.find(timer);
	if (it_tick != m_tick_timers.end()) {
		it_tick->second.cancelled = true;
		return;
	}

	auto it_pending_tick = m_pending_add_tick_timers.find(timer);
	if (it_pending_tick != m_pending_add_tick_timers.end()) {
		it_pending_tick->second.cancelled = true;
		return;
	}
}

void TimerManager::clear_timers()
{
	m_timers.clear();
	{
		decltype(m_timer_queue) empty;
		m_timer_queue.swap(empty);
	} // scope only
	m_pending_add_timers.clear();
	m_tick_timers.clear();
	m_pending_add_tick_timers.clear();
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

void TimerManager::add_tick_timer_data(TickTimerData timer_data)
{
	Timer timer = timer_data.id;
	auto res = m_tick_timers.insert(std::make_pair(timer, timer_data));
	auto success = res.second;
	assert(success);
}
