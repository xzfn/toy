#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


struct ShapeData {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<uint32_t> indices;
};

struct OutlineShapeData {
	std::vector<glm::vec3> positions;
	std::vector<uint32_t> indices;
};


ShapeData generate_unit_quad();
ShapeData generate_unit_cube();
ShapeData generate_cube();
