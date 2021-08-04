#pragma once

#include <cstdint>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "bounding_util.h"


struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct VerticesData {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
};

class MeshData {
public:
	MeshData();

	void set_vertices_data(
		const std::vector<glm::vec3>& positions,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec2>& uvs
	);
	VerticesData get_vertices_data();

	void set_indices(const std::vector<uint32_t>& indices);
	std::vector<uint32_t> get_indices();

	void set_positions(const std::vector<glm::vec3>& positions);
	void set_normals(const std::vector<glm::vec3>& normals);
	void set_uvs(const std::vector<glm::vec2>& uvs);
	std::vector<glm::vec3> get_positions();
	std::vector<glm::vec3> get_normals();
	std::vector<glm::vec2> get_uvs();

	std::vector<Vertex>& ref_vertices();
	std::vector<uint32_t>& ref_indices();

	AABB get_bounding_box();
	void recalculate_bounding_box();

private:
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	AABB m_bounding_box;
};
