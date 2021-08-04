#include "culling_util.h"


glm::vec4 normalize_plane(glm::vec4 plane) {
	glm::vec3 normal(plane);
	return plane * (1.0f / glm::length(normal));
}

bool plane_intersect_point(glm::vec4 plane, glm::vec3 point) {
	float dot = glm::dot(plane, glm::vec4(point, 1.0f));
	return dot > 0;
}

bool plane_intersect_sphere(glm::vec4 plane, glm::vec3 sphere_position, float sphere_radius) {
	float dot = glm::dot(plane, glm::vec4(sphere_position, 1.0f));
	return dot > -sphere_radius;
}

bool plane_intersect_aabb(glm::vec4 plane, AABB aabb) {
	float dot = glm::dot(plane, glm::vec4(aabb.center, 1.0f));
	float effective_radius = glm::dot(aabb.extents, glm::abs(glm::vec3(plane)));
	return dot > -effective_radius;
}

bool plane_intersect_obb(glm::vec4 plane, OBB obb) {
	float dot = glm::dot(plane, glm::vec4(obb.center, 1.0f));
	glm::vec3 normal(plane);
	glm::vec3 normal_dot_axes = normal * obb.axes;
	float effective_radius = glm::dot(obb.extents, glm::abs(normal_dot_axes));
	return dot > -effective_radius;
}

bool planes_intersect_point(std::span<glm::vec4> planes, glm::vec3 point) {
	for (auto plane : planes) {
		if (!plane_intersect_point(plane, point)) {
			return false;
		}
	}
	return true;
}

bool planes_intersect_sphere(std::span<glm::vec4> planes, glm::vec3 sphere_position, float sphere_radius) {
	for (auto plane : planes) {
		if (!plane_intersect_sphere(plane, sphere_position, sphere_radius)) {
			return false;
		}
	}
	return true;
}

bool planes_intersect_aabb(std::span<glm::vec4> planes, AABB aabb) {
	for (auto plane : planes) {
		if (!plane_intersect_aabb(plane, aabb)) {
			return false;
		}
	}
	return true;
}

bool planes_intersect_obb(std::span<glm::vec4> planes, OBB obb) {
	for (auto plane : planes) {
		if (!plane_intersect_obb(plane, obb)) {
			return false;
		}
	}
	return true;
}

std::vector<glm::vec4> view_projection_planes(glm::mat4 matrix) {
	glm::mat4 m = glm::transpose(matrix);
	glm::vec4 row0 = m[0];
	glm::vec4 row1 = m[1];
	glm::vec4 row2 = m[2];
	glm::vec4 row3 = m[3];

	glm::vec4 left = row0 + row3;
	glm::vec4 right = -row0 + row3;
	glm::vec4 bottom = row1 + row3;
	glm::vec4 top = -row1 + row3;
	glm::vec4 z_near = row2;
	glm::vec4 z_far = -row2 + row3;

	return std::vector<glm::vec4>{
		normalize_plane(left),
		normalize_plane(right),
		normalize_plane(bottom),
		normalize_plane(top),
		normalize_plane(z_near),
		normalize_plane(z_far)
	};
}

// f' = f * inverse(M)
glm::vec4 transform_plane(glm::mat4 matrix, glm::vec4 plane) {
	glm::mat4 matrix_inverse = glm::inverse(matrix);
	return normalize_plane(plane * matrix_inverse);
}

// f' = f * inverse(M), so f = f' * M
glm::vec4 inverse_transform_plane(glm::mat4 matrix, glm::vec4 plane) {
	return normalize_plane(plane * matrix);
}

std::vector<glm::vec4> clip_planes() {
	return std::vector<glm::vec4>{
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),  // left plane
		glm::vec4(-1.0f, 0.0f, 0.0f, 1.0f),  // right plane
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),  // bottom plane
		glm::vec4(0.0f, -1.0f, 0.0f, 1.0f),  // top plane
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),  // near plane
		glm::vec4(0.0f, 0.0f, -1.0f, 1.0f)  // far plane
	};
}
