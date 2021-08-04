#include "bounding_util.h"


AABB min_max_to_aabb(glm::vec3 mins, glm::vec3 maxs) {
	glm::vec3 center = (mins + maxs) * 0.5f;
	glm::vec3 extents = (maxs - mins) * 0.5f;
	return AABB{
		center, extents
	};
}


AABB transform_aabb(glm::mat4 matrix, AABB aabb) {
	// from Jim Arvo Graphics Gems (1990), Real-Time Collision Detection 4.2
	glm::vec3 old_center = aabb.center;
	glm::vec3 old_extents = aabb.extents;
	glm::vec3 new_center(matrix[3][0], matrix[3][1], matrix[3][2]);
	glm::vec3 new_extents(VEC3_ZERO);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			new_center[i] += matrix[j][i] * old_center[j];
			new_extents[i] += std::abs(matrix[j][i]) * old_extents[j];
		}
	}
	return AABB{
		new_center, new_extents
	};
}


AABB points_to_aabb(std::span<const glm::vec3> points) {
	assert(points.size() > 0);
	glm::vec3 mins(points[0]);
	glm::vec3 maxs(points[0]);
	for (auto point : points) {
		mins = glm::min(mins, point);
		maxs = glm::max(maxs, point);
	}
	return min_max_to_aabb(mins, maxs);
}


OBB aabb_to_obb(glm::mat4 matrix, AABB aabb) {
	glm::vec3 new_center = matrix * glm::vec4(aabb.center, 1.0f);
	return OBB{
		new_center,
		glm::mat3(matrix),
		aabb.extents
	};
}

void PointsToAABB::add(glm::vec3 point)
{
	mins = glm::min(mins, point);
	maxs = glm::max(maxs, point);
}

AABB PointsToAABB::get()
{
	return min_max_to_aabb(mins, maxs);
}
