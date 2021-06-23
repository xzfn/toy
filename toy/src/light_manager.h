#pragma once

#include <vector>
#include <memory>

#include "light.h"


constexpr int MAX_LIGHTS = 8;


struct LightUniform {
	LightType type;
	float padding_0;
	float padding_1;
	float padding_2;

	glm::vec3 color;
	float padding_3;

	glm::vec3 position;
	float padding_4;

	glm::vec3 direction;
	float padding_5;
};

struct LightUniforms {
	int light_count;
	float padding_0;
	float padding_1;
	float padding_2;

	LightUniform lights[MAX_LIGHTS];
};


class LightManager {
public:
	LightManager();
	LightManager(const LightManager&) = delete;
	LightManager& operator=(const LightManager&) = delete;

public:
	void add_light(std::shared_ptr<Light> light);
	void remove_light(std::shared_ptr<Light> light);

	LightUniforms build_light_uniform();

public:
	std::vector<std::shared_ptr<Light>> m_lights;
};
