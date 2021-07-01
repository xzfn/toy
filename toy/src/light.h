#pragma once

#include <glm/vec3.hpp>

#include "glm_util.h"


enum class LightType {
	Empty = 0,
	Point = 1,
	Spot = 2,
	Directional = 3
};

struct LightData {
	LightType type = LightType::Empty;
	glm::vec3 color;
	glm::vec3 position = VEC3_ZERO;
	glm::vec3 direction;
	float range = 50.0f;
	float spot_outer_angle;
	float spot_inner_angle;
	bool shadow = true;
	int shadow_layer = 0;
};

class Light {
public:
	Light();
	Light(const Light&) = delete;
	Light& operator=(const Light&) = delete;

public:
	LightType get_type();
	void set_type(LightType type);

	glm::vec3 get_position();
	void set_position(glm::vec3 position);

	glm::vec3 get_color();
	void set_color(glm::vec3 color);

	glm::vec3 get_direction();
	void set_direction(glm::vec3 direction);

	float get_range();
	void set_range(float range);

	float get_spot_outer_angle();
	void set_spot_outer_angle(float angle);

	float get_spot_inner_angle();
	void set_spot_inner_angle(float angle);

	bool get_shadow();
	void set_shadow(bool shadow);

	void internal_set_shadow_layer(int layer);

public:
	LightData data;
};
