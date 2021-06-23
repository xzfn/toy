#include <algorithm>
#include "light_manager.h"


LightManager::LightManager()
{
}

void LightManager::add_light(std::shared_ptr<Light> light)
{
	m_lights.push_back(light);
}

void LightManager::remove_light(std::shared_ptr<Light> light)
{
	auto it = std::find(m_lights.begin(), m_lights.end(), light);
	if (it != m_lights.end()) {
		m_lights.erase(it);
	}
}

LightUniforms LightManager::build_light_uniform()
{
	LightUniforms light_uniforms;
	int light_index = 0;
	int actual_index = 0;
	for (int i = 0; i < m_lights.size(); ++i) {
		Light& light = *m_lights[i];
		if (light.get_type() == LightType::Empty) {
			continue;
		}
		if (actual_index >= MAX_LIGHTS) {
			break;
		}
		
		LightUniform uniform;
		uniform.type = light.data.type;
		uniform.color = light.data.color;
		uniform.position = light.data.position;
		uniform.direction = light.data.direction;
		light_uniforms.lights[actual_index] = uniform;
		++actual_index;
	}
	light_uniforms.light_count = actual_index;
	return light_uniforms;
}
