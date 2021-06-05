#include "text_builder.h"

#include <iostream>


void TextBuilder::clear()
{
	m_texts.clear();
}

void TextBuilder::add_text(glm::vec3 position, std::string text, glm::vec3 color)
{
	TextInfo info{
		position,
		text,
		color
	};
	m_texts.push_back(info);
}

void write_text_triangles(TextInfo info, std::vector<ColorTextureTriangleData>& buffer) {
	int extent = 512;
	int char_width = 30;
	int char_height = 52;
	int col_gap = 40;
	int row_gap = 60;
	int ncols = 12;
	int nrows = 8;
	int anchor_x = 17;
	int anchor_y = 73;
	char char_start = 32;
	char char_stop = 0x7e + 1;

	float du = 1.0f * char_width / extent;
	float dv = 1.0f * char_height / extent;

	float pos_dx = char_width / 10.0f;
	float pos_dy = char_height / 10.0f;

	float pos_x = 0.0f;
	float pos_y = 0.0f;

	for (char c : info.text) {
		char i = c - char_start;
		int row = i / ncols;
		int col = i % ncols;
		int x = anchor_x + col_gap * col;
		int y = anchor_y + row_gap * row - char_height;
		
		float u1 = 1.0f * x / extent;
		float v1 = 1.0f * y / extent;
		float u2 = u1 + du;
		float v2 = v1 + dv;
		
		float pos_x2 = pos_x + pos_dx;
		float pos_y2 = pos_y + pos_dy;

		// pos_x, pos_y lower-left; pos_x2, pos_y2 upper-right
		// u1, v2 lower-left; u2 v1 upper-right

		ColorTextureVertex vertex_a{
			info.position + glm::vec3(pos_x, pos_y, 0.0f),
			info.color,
			glm::vec2(u1, v2)
		};

		ColorTextureVertex vertex_b{
			info.position + glm::vec3(pos_x2, pos_y, 0.0f),
			info.color,
			glm::vec2(u2, v2)
		};

		ColorTextureVertex vertex_c{
			info.position + glm::vec3(pos_x, pos_y2, 0.0f),
			info.color,
			glm::vec2(u1, v1)
		};

		ColorTextureVertex vertex_d{
			info.position + glm::vec3(pos_x2, pos_y2, 0.0f),
			info.color,
			glm::vec2(u2, v1)
		};

		ColorTextureTriangleData triangle1{
			vertex_a, vertex_b, vertex_c
		};

		ColorTextureTriangleData triangle2{
			vertex_c, vertex_b, vertex_d
		};

		buffer.push_back(triangle1);
		buffer.push_back(triangle2);


		pos_x += pos_dx;
	}
}

std::vector<ColorTextureTriangleData> TextBuilder::build_buffer()
{
	std::vector<ColorTextureTriangleData> buffer;
	for (auto& text : m_texts) {
		write_text_triangles(text, buffer);
	}
	return buffer;
}
