#pragma once

#include <vector>
#include <cstdint>
#include <functional>

#include <glm/vec3.hpp>


typedef std::function<void(glm::vec3 point)> CallbackPoint;
typedef std::function<void(glm::vec3 point0, glm::vec3 point1)> CallbackLine;


std::vector<glm::vec3> generate_circle_points(float radius, uint32_t segments, float y = 0.0f);

std::vector<glm::vec3> connect_point_pairs(std::vector<glm::vec3>& points0, std::vector<glm::vec3>& points1);

std::vector<glm::vec3> connect_points_tip(std::vector<glm::vec3>& points0, glm::vec3 tip);

std::vector<glm::vec3> connect_points_loop(std::vector<glm::vec3>& points);

std::vector<glm::vec3> offset_points(std::vector<glm::vec3> points, glm::vec3 offset);
