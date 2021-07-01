#include "light.h"

Light::Light()
{
}

LightType Light::get_type()
{
	return data.type;
}

void Light::set_type(LightType type)
{
	data.type = type;
}

glm::vec3 Light::get_position()
{
	return data.position;
}

void Light::set_position(glm::vec3 position)
{
	data.position = position;
}

glm::vec3 Light::get_color()
{
	return data.color;
}

void Light::set_color(glm::vec3 color)
{
	data.color = color;
}

glm::vec3 Light::get_direction()
{
	return data.direction;
}

void Light::set_direction(glm::vec3 direction)
{
	data.direction = direction;
}

float Light::get_range() {
	return data.range;
}

void Light::set_range(float range) {
	data.range = range;
}

float Light::get_spot_outer_angle() {
	return data.spot_outer_angle;
}

void Light::set_spot_outer_angle(float angle) {
	data.spot_outer_angle = angle;
}

float Light::get_spot_inner_angle() {
	return data.spot_inner_angle;
}

void Light::set_spot_inner_angle(float angle) {
	data.spot_inner_angle = angle;
}

bool Light::get_shadow() {
	return data.shadow;
}

void Light::set_shadow(bool shadow) {
	data.shadow = shadow;
}

void Light::internal_set_shadow_layer(int layer) {
	data.shadow_layer = layer;
}
