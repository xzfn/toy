#pragma once

#include "timed_info_fwd.h"

#include <vector>
#include <algorithm>

namespace timedinfo {

template <typename T>
void update_info_duration(std::vector<TimedInfo<T>>& m_infos, float delta_time)
{
	for (auto& info : m_infos) {
		info.duration -= delta_time;
	}
}

template <typename T>
bool is_expired(const TimedInfo<T>& info)
{
	return info.duration < 0.0f;
}

template <typename T>
void remove_expired_info(std::vector<TimedInfo<T>>& m_infos)
{
	auto it = std::remove_if(m_infos.begin(), m_infos.end(), is_expired<T>);
	m_infos.erase(it, m_infos.end());
}

template <typename T>
void process_timed_info(std::vector<TimedInfo<T>>& m_infos, float delta_time)
{
	update_info_duration(m_infos, delta_time);
	remove_expired_info(m_infos);
}


}
