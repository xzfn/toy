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
