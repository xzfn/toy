#include "text_builder.h"

#include <iostream>

#include "ascii_texture.h"
#include "timed_info.h"
#include "global_app.h"


void write_screen_text_triangles(TextInfo info, std::vector<ColorTextureTriangleData>& buffer);


void TextBuilder::clear()
{
	m_texts.clear();
	m_screen_texts.clear();
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

void TextBuilder::add_screen_text(glm::vec3 position, std::string text, glm::vec3 color)
{
	TextInfo info{
		position,
		text,
		color
	};
	m_screen_texts.push_back(info);
}

void write_text_triangles(TextInfo info, std::vector<ColorTextureTriangleData>& buffer) {
	glm::vec3 world_position = info.position;
	Camera* camera = get_app()->camera_manager.get_camera();
	glm::vec3 screen_position = camera->world_to_screen(world_position);

	TextInfo screen_text_info{
		screen_position,
		info.text,
		info.color
	};
	write_screen_text_triangles(screen_text_info, buffer);
}

void write_screen_text_triangles(TextInfo info, std::vector<ColorTextureTriangleData>& buffer) {
	int char_width = ASCIITextureInfo::char_width;
	int char_height = ASCIITextureInfo::char_height;

	float spacing_scale = 0.8f;

	float pos_dx = char_width * 0.5 * spacing_scale;
	float pos_dy = char_height * 0.5 * spacing_scale;

	float pos_x = 0.0f;
	float pos_y = 0.0f;

	float screen_height = get_app()->ctxptr->basic.extent.height;

	glm::vec3 base_position = glm::vec3(info.position.x, screen_height - info.position.y, info.position.z);

	for (char c : info.text) {
		if (c == '\n') {
			pos_x = 0.0f;
			pos_y -= pos_dy;
			continue;
		}

		CharUv char_uv = get_char_uv(c);
		float u1 = char_uv.u1;
		float v1 = char_uv.v1;
		float u2 = char_uv.u2;
		float v2 = char_uv.v2;

		float pos_x2 = pos_x + pos_dx;
		float pos_y2 = pos_y + pos_dy;

		// pos_x, pos_y lower-left; pos_x2, pos_y2 upper-right
		// u1, v2 lower-left; u2 v1 upper-right

		ColorTextureVertex vertex_a{
			base_position + glm::vec3(pos_x, pos_y, 0.0f),
			info.color,
			glm::vec2(u1, v2)
		};

		ColorTextureVertex vertex_b{
			base_position + glm::vec3(pos_x2, pos_y, 0.0f),
			info.color,
			glm::vec2(u2, v2)
		};

		ColorTextureVertex vertex_c{
			base_position + glm::vec3(pos_x, pos_y2, 0.0f),
			info.color,
			glm::vec2(u1, v1)
		};

		ColorTextureVertex vertex_d{
			base_position + glm::vec3(pos_x2, pos_y2, 0.0f),
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

	for (auto& text : m_screen_texts) {
		write_screen_text_triangles(text, buffer);
	}
	return buffer;
}

void TimedTextBuilder::clear()
{
	m_texts.clear();
	m_screen_texts.clear();
}

void TimedTextBuilder::add_text(glm::vec3 position, std::string text, glm::vec3 color, float duration)
{
	TextInfo info{
		position,
		text,
		color
	};
	timedinfo::TimedInfo<TextInfo> timed_info{
		info,
		duration
	};
	m_texts.push_back(timed_info);
}

void TimedTextBuilder::add_screen_text(glm::vec3 position, std::string text, glm::vec3 color, float duration)
{
	TextInfo info{
		position,
		text,
		color
	};
	timedinfo::TimedInfo<TextInfo> timed_info{
		info,
		duration
	};
	m_screen_texts.push_back(timed_info);
}

void TimedTextBuilder::update(float delta_time)
{
	timedinfo::process_timed_info(m_texts, delta_time);
	timedinfo::process_timed_info(m_screen_texts, delta_time);
}

std::vector<ColorTextureTriangleData> TimedTextBuilder::build_buffer()
{
	std::vector<ColorTextureTriangleData> buffer;
	for (auto& text : m_texts) {
		write_text_triangles(text.info, buffer);
	}

	for (auto& text : m_screen_texts) {
		write_screen_text_triangles(text.info, buffer);
	}
	return buffer;
}
