#pragma once

#include <volk.h>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>


enum class VertexFormat {
	PositionNormalUv,
	PositionColor,
};

struct VertexPositionNormalUv {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct VertexPositionColor {
	glm::vec3 position;
	glm::vec3 color;
};

VkPipelineVertexInputStateCreateInfo generate_vertex_input_state(VertexFormat vertex_format);
