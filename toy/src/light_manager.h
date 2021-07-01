#pragma once

#include <vector>
#include <memory>

#include "light.h"


constexpr int MAX_LIGHTS = 8;


struct LightUniform {
	LightType type;
	int shadow;
	int shadow_layer;
	float padding_0;

	glm::vec3 color;
	float padding_3;

	glm::vec3 position;
	float padding_4;

	glm::vec3 direction;
	float padding_5;

	float range;
	float spot_inner;
	float spot_outer;
	float padding_6;
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
	void set_sun(std::shared_ptr<Light> light);
	std::shared_ptr<Light> get_sun();

	void add_light(std::shared_ptr<Light> light);
	void remove_light(std::shared_ptr<Light> light);

	LightUniforms build_light_uniform();
	std::vector<std::shared_ptr<Light>>& ref_lights();

public:
	std::shared_ptr<Light> m_sun;
	std::vector<std::shared_ptr<Light>> m_lights;
};
