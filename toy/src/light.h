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
	glm::vec3 color = VEC3_ONES;
	Transform transform;
	float range = 50.0f;
	float spot_outer_angle = 1.0f;
	float spot_inner_angle = 0.5f;
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

	Transform get_transform();
	void set_transform(Transform transform);

	glm::vec3 get_color();
	void set_color(glm::vec3 color);

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
