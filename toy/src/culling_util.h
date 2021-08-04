#pragma once

#include <span>
#include <vector>

#include "glm_util.h"
#include "bounding_util.h"


glm::vec4 normalize_plane(glm::vec4 plane);


bool plane_intersect_point(glm::vec4 plane, glm::vec3 point);

bool plane_intersect_sphere(glm::vec4 plane, glm::vec3 sphere_position, float sphere_radius);

bool plane_intersect_aabb(glm::vec4 plane, AABB aabb);

bool plane_intersect_obb(glm::vec4 plane, OBB obb);

bool planes_intersect_point(std::span<glm::vec4> planes, glm::vec3 point);

bool planes_intersect_sphere(std::span<glm::vec4> planes, glm::vec3 sphere_position, float sphere_radius);

bool planes_intersect_aabb(std::span<glm::vec4> planes, AABB aabb);

bool planes_intersect_obb(std::span<glm::vec4> planes, OBB obb);


std::vector<glm::vec4> view_projection_planes(glm::mat4 matrix);

glm::vec4 transform_plane(glm::mat4 matrix, glm::vec4 plane);

glm::vec4 inverse_transform_plane(glm::mat4 matrix, glm::vec4 plane);

std::vector<glm::vec4> clip_planes();
