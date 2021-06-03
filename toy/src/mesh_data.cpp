#include "mesh_data.h"

#include <cassert>


MeshData::MeshData()
{
}

void MeshData::set_vertices_data(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs)
{
	set_positions(positions);
	set_normals(normals);
	set_uvs(uvs);
}

VerticesData MeshData::get_vertices_data()
{
	return VerticesData{
		get_positions(),
		get_normals(),
		get_uvs()
	};
}

void MeshData::set_indices(const std::vector<uint32_t>& indices)
{
	m_indices = indices;
}

std::vector<uint32_t> MeshData::get_indices()
{
	return m_indices;
}

void MeshData::set_positions(const std::vector<glm::vec3>& positions)
{
	std::size_t vertex_count = positions.size();
	m_vertices.resize(vertex_count);
	memset(m_vertices.data(), 0, vertex_count * sizeof(Vertex));
	for (std::size_t i = 0; i < vertex_count; ++i) {
		m_vertices[i].position = positions[i];
	}
}

void MeshData::set_normals(const std::vector<glm::vec3>& normals)
{
	std::size_t vertex_count = m_vertices.size();
	if (normals.size() == vertex_count) {
		for (std::size_t i = 0; i < vertex_count; ++i) {
			m_vertices[i].normal = normals[i];
		}
	}
}

void MeshData::set_uvs(const std::vector<glm::vec2>& uvs)
{
	std::size_t vertex_count = m_vertices.size();
	if (uvs.size() == vertex_count) {
		for (std::size_t i = 0; i < vertex_count; ++i) {
			m_vertices[i].uv = uvs[i];
		}
	}
}

std::vector<glm::vec3> MeshData::get_positions()
{
	std::size_t vertex_count = m_vertices.size();
	std::vector<glm::vec3> positions(vertex_count);
	for (std::size_t i = 0; i < vertex_count; ++i) {
		positions[i] = m_vertices[i].position;
	}
	return positions;
}

std::vector<glm::vec3> MeshData::get_normals()
{
	std::size_t vertex_count = m_vertices.size();
	std::vector<glm::vec3> normals(vertex_count);
	for (std::size_t i = 0; i < vertex_count; ++i) {
		normals[i] = m_vertices[i].normal;
	}
	return normals;
}

std::vector<glm::vec2> MeshData::get_uvs()
{
	std::size_t vertex_count = m_vertices.size();
	std::vector<glm::vec2> uvs(vertex_count);
	for (std::size_t i = 0; i < vertex_count; ++i) {
		uvs[i] = m_vertices[i].uv;
	}
	return uvs;
}

std::vector<Vertex>& MeshData::ref_vertices()
{
	return m_vertices;
}

std::vector<uint32_t>& MeshData::ref_indices()
{
	return m_indices;
}
