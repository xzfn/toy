#pragma once

#include <volk.h>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>



enum class VertexFormat {
	PositionNormalUv,
	PositionColor,
	PositionColorUv,
	ImGuiPositionUvColor,
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

struct VertexPositionColorUv {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 uv;
};

struct VertexImGui {
	glm::vec2 position;
	glm::vec2 uv;
	glm::u8vec4 color;
};

VkPipelineVertexInputStateCreateInfo generate_vertex_input_state(VertexFormat vertex_format);
