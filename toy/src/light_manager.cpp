
#include "light_manager.h"

#include <cmath>
#include <algorithm>

#include <glm/glm.hpp>


LightManager::LightManager()
{
	auto sun = std::make_shared<Light>();
	Transform sun_transform;
	sun_transform.rotation = look_rotation_to_quat(glm::normalize(glm::vec3(0.0f, -1.0f, 0.5f)), VEC3_Y);
	sun->set_transform(sun_transform);
	sun->set_color(glm::vec3(1.0f, 1.0f, 1.0f));
	set_sun(sun);
}

void LightManager::set_sun(std::shared_ptr<Light> light)
{
	m_sun = light;
}

std::shared_ptr<Light> LightManager::get_sun()
{
	return m_sun;
}

void LightManager::add_light(std::shared_ptr<Light> light)
{
	auto it = std::find(m_lights.begin(), m_lights.end(), light);
	if (it != m_lights.end()) {
		return;
	}
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
	int actual_index = 0;
	for (int i = 0; i < m_lights.size(); ++i) {
		Light& light = *m_lights[i];
		if (light.get_type() == LightType::Empty) {
			continue;
		}
		if (actual_index >= MAX_LIGHTS) {
			break;
		}
		LightData& light_data = light.data;
		LightUniform uniform;
		uniform.type = light_data.type;
		uniform.color = light_data.color;
		uniform.position = light_data.transform.translation;
		uniform.direction = -quat_axis_z(light_data.transform.rotation);
		uniform.range = light_data.range;
		uniform.spot_inner = std::cos(light_data.spot_inner_angle * 0.5f);
		uniform.spot_outer = std::cos(light_data.spot_outer_angle * 0.5f);

		int shadow_layer = light_data.shadow_layer;
		uniform.shadow_layer = shadow_layer;
		if (shadow_layer == -1) {
			uniform.shadow = false;
		}
		else {
			uniform.shadow = light_data.shadow;
		}

		light_uniforms.lights[actual_index] = uniform;
		++actual_index;
	}
	light_uniforms.light_count = actual_index;
	return light_uniforms;
}

std::vector<std::shared_ptr<Light>>& LightManager::ref_lights()
{
	return m_lights;
}
