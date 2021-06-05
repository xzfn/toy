#pragma once

#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "timed_info_fwd.h"

#include "color_util.h"


struct TextInfo {
	glm::vec3 position;
	std::string text;
	glm::vec3 color;
};

struct ColorTextureVertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 uv;
};

struct ColorTextureTriangleData {
	ColorTextureVertex vertices[3];
};

constexpr glm::vec3 TEXT_DEFAULT_COLOR = COLOR_RED;

class TextBuilder {
public:
	void clear();
	void add_text(glm::vec3 position, std::string text, glm::vec3 color=TEXT_DEFAULT_COLOR);

	std::vector<ColorTextureTriangleData> build_buffer();

private:
	std::vector<TextInfo> m_texts;
};
