#pragma once

#include <glm/vec3.hpp>


enum class LightType {
	Empty = 0,
	Point = 1,
	Spot = 2,
	Directional = 3
};

struct LightData {
	LightType type = LightType::Empty;
	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 direction;	
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

public:
	LightData data;
};
