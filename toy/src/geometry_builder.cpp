
#define _USE_MATH_DEFINES
#include <cmath>

#include "geometry_builder.h"

#include <algorithm>
#include <map>

#include "geometry_util.h"

namespace numbers{
	constexpr float pi = M_PI;
	constexpr float pi_mul_2 = pi * 2.0f;
	constexpr float pi_div_2 = pi * 0.5f;
	constexpr float pi_div_4 = pi * 0.25f;
}


void GeometryBuilder::clear()
{
	m_lines.clear();
	m_cubes.clear();
	m_spheres.clear();
}

void GeometryBuilder::add_line(glm::vec3 position0, glm::vec3 position1, glm::vec3 color)
{
	LineInfo info{
		position0,
		position1,
		color
	};
	m_lines.push_back(info);
}

void GeometryBuilder::add_cube(glm::vec3 position, float length, glm::vec3 color)
{
	CubeInfo info{
		position,
		length,
		color
	};
	m_cubes.push_back(info);
}

void GeometryBuilder::add_sphere(glm::vec3 position, float radius, glm::vec3 color)
{
	SphereInfo info{
		position,
		radius,
		color
	};
	m_spheres.push_back(info);
}

void write_line_lines(LineInfo line, std::vector<ColorLineData>& buffer) {
	ColorLineData line_data{
		line.position0,
		line.color,
		line.position1,
		line.color
	};
	buffer.push_back(line_data);
}

void write_cube_lines(CubeInfo cube, std::vector<ColorLineData>& buffer) {
	glm::vec3 position = cube.position;
	glm::vec3 color = cube.color;
	float half_length = cube.length * 0.5f;
	glm::vec3 vertices_lower[4] = {
		position + glm::vec3(half_length, -half_length, half_length),
		position + glm::vec3(-half_length, -half_length, half_length),
		position + glm::vec3(-half_length, -half_length, -half_length),
		position + glm::vec3(half_length, -half_length, -half_length)
	};
	glm::vec3 vertices_upper[4] = {
		position + glm::vec3(half_length, half_length, half_length),
		position + glm::vec3(-half_length, half_length, half_length),
		position + glm::vec3(-half_length, half_length, -half_length),
		position + glm::vec3(half_length, half_length, -half_length),
	};

	buffer.push_back(ColorLineData{ vertices_lower[0], color, vertices_lower[1], color });
	buffer.push_back(ColorLineData{ vertices_lower[1], color, vertices_lower[2], color });
	buffer.push_back(ColorLineData{ vertices_lower[2], color, vertices_lower[3], color });
	buffer.push_back(ColorLineData{ vertices_lower[3], color, vertices_lower[0], color });
	
	buffer.push_back(ColorLineData{ vertices_upper[0], color, vertices_upper[1], color });
	buffer.push_back(ColorLineData{ vertices_upper[1], color, vertices_upper[2], color });
	buffer.push_back(ColorLineData{ vertices_upper[2], color, vertices_upper[3], color });
	buffer.push_back(ColorLineData{ vertices_upper[3], color, vertices_upper[0], color });

	buffer.push_back(ColorLineData{ vertices_lower[0], color, vertices_upper[0], color });
	buffer.push_back(ColorLineData{ vertices_lower[1], color, vertices_upper[1], color });
	buffer.push_back(ColorLineData{ vertices_lower[2], color, vertices_upper[2], color });
	buffer.push_back(ColorLineData{ vertices_lower[3], color, vertices_upper[3], color });
}

void write_raw_lines(std::vector<glm::vec3>& raw_lines, glm::vec3 color, std::vector<ColorLineData>& buffer)
{
	std::size_t n_lines = raw_lines.size() / 2;
	for (std::size_t i = 0; i < n_lines; ++i) {
		buffer.push_back(ColorLineData{ raw_lines[i*2], color, raw_lines[i*2+1], color });
	}
}


std::vector<glm::vec3> generate_sphere_raw_lines(float radius)
{
	uint32_t segments = 8;
	float lat_radius = radius * std::sinf(numbers::pi_div_4);
	std::vector<glm::vec3> equator_points = generate_circle_points(radius, segments);
	std::vector<glm::vec3> upper_points = generate_circle_points(lat_radius, segments, lat_radius);
	std::vector<glm::vec3> lower_points = generate_circle_points(lat_radius, segments, -lat_radius);
	glm::vec3 top_tip = glm::vec3(0.0f, radius, 0.0f);
	glm::vec3 bottom_tip = glm::vec3(0.0f, -radius, 0.0f);

	std::vector<glm::vec3> raw_lines;
	std::vector<glm::vec3> temp_lines;

	temp_lines = connect_points_loop(upper_points);
	raw_lines.insert(raw_lines.end(), temp_lines.begin(), temp_lines.end());

	temp_lines = connect_points_loop(equator_points);
	raw_lines.insert(raw_lines.end(), temp_lines.begin(), temp_lines.end());

	temp_lines = connect_points_loop(lower_points);
	raw_lines.insert(raw_lines.end(), temp_lines.begin(), temp_lines.end());

	temp_lines = connect_points_tip(upper_points, top_tip);
	raw_lines.insert(raw_lines.end(), temp_lines.begin(), temp_lines.end());
	temp_lines = connect_point_pairs(upper_points, equator_points);
	raw_lines.insert(raw_lines.end(), temp_lines.begin(), temp_lines.end());
	temp_lines = connect_point_pairs(equator_points, lower_points);
	raw_lines.insert(raw_lines.end(), temp_lines.begin(), temp_lines.end());
	temp_lines = connect_points_tip(lower_points, bottom_tip);
	raw_lines.insert(raw_lines.end(), temp_lines.begin(), temp_lines.end());
	return raw_lines;
}

std::map<float, std::vector<glm::vec3>> g_sphere_raw_lines_cache;

bool is_float_integer(float f) {
	float integral;
	return std::modf(f, &integral) == 0.0f;
}

void write_sphere_lines(SphereInfo sphere, std::vector<ColorLineData>& buffer)
{
	glm::vec3 position = sphere.position;
	glm::vec3 color = sphere.color;
	float radius = sphere.radius;
	
	std::vector<glm::vec3> raw_lines;

	auto it = g_sphere_raw_lines_cache.find(radius);
	if (it != g_sphere_raw_lines_cache.end()) {
		raw_lines = it->second;
	}
	else {
		raw_lines = generate_sphere_raw_lines(radius);
		if (is_float_integer(radius)) {
			g_sphere_raw_lines_cache[radius] = raw_lines;
		}
	}

	raw_lines = offset_points(raw_lines, position);
	write_raw_lines(raw_lines, color, buffer);
}

std::vector<ColorLineData> GeometryBuilder::build_buffer()
{
	std::vector<ColorLineData> buffer;
	for (auto& line : m_lines) {
		write_line_lines(line, buffer);
	}

	for (auto& cube : m_cubes) {
		write_cube_lines(cube, buffer);
	}

	for (auto& sphere : m_spheres) {
		write_sphere_lines(sphere, buffer);
	}
	return buffer;
}



void TimedGeometryBuilder::clear()
{
	m_lines.clear();
	m_cubes.clear();
	m_spheres.clear();
}

void TimedGeometryBuilder::add_line(glm::vec3 position0, glm::vec3 position1, glm::vec3 color, float duration)
{
	LineInfo info{
		position0,
		position1,
		color
	};
	TimedInfo<LineInfo> timed_info{
		info,
		duration
	};
	m_lines.push_back(timed_info);
}

void TimedGeometryBuilder::add_cube(glm::vec3 position, float length, glm::vec3 color, float duration)
{
	CubeInfo info{
		position,
		length,
		color
	};
	TimedInfo<CubeInfo> timed_info{
		info,
		duration
	};
	m_cubes.push_back(timed_info);
}

void TimedGeometryBuilder::add_sphere(glm::vec3 position, float radius, glm::vec3 color, float duration)
{
	SphereInfo info{
		position,
		radius,
		color
	};
	TimedInfo<SphereInfo> timed_info{
		info,
		duration
	};
	m_spheres.push_back(timed_info);
}

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

void TimedGeometryBuilder::update(float delta_time)
{
	process_timed_info(m_lines, delta_time);
	process_timed_info(m_cubes, delta_time);
	process_timed_info(m_spheres, delta_time);
}

std::vector<ColorLineData> TimedGeometryBuilder::build_buffer()
{
	std::vector<ColorLineData> buffer;
	for (auto& line : m_lines) {
		write_line_lines(line.info, buffer);
	}

	for (auto& cube : m_cubes) {
		write_cube_lines(cube.info, buffer);
	}

	for (auto& sphere : m_spheres) {
		write_sphere_lines(sphere.info, buffer);
	}
	return buffer;
}
