#pragma once

#include <cstdint>
#include <span>

#include "glm_util.h"


struct AABB {
	glm::vec3 center;
	glm::vec3 extents;
};


struct OBB {
	glm::vec3 center;
	glm::mat3 axes;
	glm::vec3 extents;
};


std::string aabb_to_string(AABB aabb);


AABB min_max_to_aabb(glm::vec3 mins, glm::vec3 maxs);

AABB transform_aabb(glm::mat4 matrix, AABB aabb);

AABB points_to_aabb(std::span<const glm::vec3> points);

struct PointsToAABB {
	void add(glm::vec3 point);
	glm::vec3 mins{ (float)FLT_MAX };
	glm::vec3 maxs{ -(float)FLT_MAX };
	AABB get();
};

OBB aabb_to_obb(glm::mat4 matrix, AABB aabb);
