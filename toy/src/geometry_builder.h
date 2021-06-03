#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include "color_util.h"


enum class GeometryType {
	Line,
	Cube,
	Sphere,
};

struct LineInfo {
	glm::vec3 position0;
	glm::vec3 position1;
	glm::vec3 color;
};

struct CubeInfo {
	glm::vec3 position;
	float length;
	glm::vec3 color;
};

struct SphereInfo {
	glm::vec3 position;
	float radius;
	glm::vec3 color;
};

struct GeometryInfo {
	GeometryType type;
	union
	{
		LineInfo line;
		CubeInfo cube;
		SphereInfo sphere;
	};
};

struct ColorLineData {
	glm::vec3 position0;
	glm::vec3 color0;
	glm::vec3 position1;
	glm::vec3 color1;
};

constexpr glm::vec3 GEOMETRY_DEFAULT_COLOR = COLOR_RED;

class GeometryBuilder {
public:
	void clear();

	void add_line(glm::vec3 position0, glm::vec3 position1, glm::vec3 color=GEOMETRY_DEFAULT_COLOR);
	void add_cube(glm::vec3 position, float length, glm::vec3 color=GEOMETRY_DEFAULT_COLOR);
	void add_sphere(glm::vec3 position, float radius, glm::vec3 color=GEOMETRY_DEFAULT_COLOR);

	std::vector<ColorLineData> build_buffer();

private:
	std::vector<LineInfo> m_lines;
	std::vector<CubeInfo> m_cubes;
	std::vector<SphereInfo> m_spheres;
};


template <typename T>
struct TimedInfo {
	T info;
	float duration;
};

class TimedGeometryBuilder {
public:
	void clear();

	void add_line(glm::vec3 position0, glm::vec3 position1, glm::vec3 color=GEOMETRY_DEFAULT_COLOR, float duration=0.0f);
	void add_cube(glm::vec3 position, float length, glm::vec3 color=GEOMETRY_DEFAULT_COLOR, float duration=0.0f);
	void add_sphere(glm::vec3 position, float radius, glm::vec3 color=GEOMETRY_DEFAULT_COLOR, float duration=0.0f);

	void update(float delta_time);

	std::vector<ColorLineData> build_buffer();

private:
	std::vector<TimedInfo<LineInfo>> m_lines;
	std::vector<TimedInfo<CubeInfo>> m_cubes;
	std::vector<TimedInfo<SphereInfo>> m_spheres;
};
