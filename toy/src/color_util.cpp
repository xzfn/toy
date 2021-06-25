#include "color_util.h"

#include <glm/common.hpp>


uint8_t color_component_to_byte(float c) {
	c = glm::clamp(c, 0.0f, 1.0f);
	return (uint8_t)(c * 255);
}

uint32_t rgb_to_hex(glm::vec3 color) {
	// 0xAABBGGRR
	uint8_t a = 0xff;
	uint8_t r = color_component_to_byte(color.x);
	uint8_t g = color_component_to_byte(color.y);
	uint8_t b = color_component_to_byte(color.z);
	return (a << 24) | (b << 16) | (g << 8) | r;
}
