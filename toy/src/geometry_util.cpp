
#define _USE_MATH_DEFINES
#include <cmath>

#include "geometry_util.h"

#include <cassert>


namespace numbers{
	constexpr float pi = (float)M_PI;
	constexpr float pi_mul_2 = pi * 2.0f;
	constexpr float pi_div_2 = pi * 0.5f;
	constexpr float pi_div_4 = pi * 0.25f;
}

std::vector<glm::vec3> generate_circle_points(float radius, uint32_t segments, float y)
{
	std::vector<glm::vec3> points;
	points.reserve(segments);
	float delta_angle = numbers::pi_mul_2 / segments;
	for (uint32_t i = 0; i < segments; ++i) {
		float theta = delta_angle * i;
		float x = radius * std::cosf(theta);
		float z = radius * std::sinf(theta);
		points.push_back(glm::vec3(x, y, z));
	}
	return points;
}

std::vector<glm::vec3> connect_point_pairs(std::vector<glm::vec3>& points0, std::vector<glm::vec3>& points1)
{
	assert(points0.size() == points1.size());
	
	std::vector<glm::vec3> lines;
	for (std::size_t i = 0; i < points0.size(); ++i) {
		glm::vec3 point0 = points0[i];
		glm::vec3 point1 = points1[i];
		lines.push_back(point0);
		lines.push_back(point1);
	}
	return lines;
}

std::vector<glm::vec3> connect_points_tip(std::vector<glm::vec3>& points, glm::vec3 tip)
{
	std::vector<glm::vec3> lines;
	for (std::size_t i = 0; i < points.size(); ++i) {
		glm::vec3 point = points[i];
		lines.push_back(tip);
		lines.push_back(point);
	}
	return lines;
}

std::vector<glm::vec3> connect_points_loop(std::vector<glm::vec3>& points)
{
	std::vector<glm::vec3> lines;
	std::size_t point_count = points.size();
	for (std::size_t i = 0; i < point_count; ++i) {
		lines.push_back(points[i]);
		lines.push_back(points[(i + 1) % point_count]);
	}
	return lines;
}

std::vector<glm::vec3> offset_points(std::vector<glm::vec3> points, glm::vec3 offset)
{
	for (auto& point : points) {
		point += offset;
	}
	return points;
}
