#include "mesh.h"
#include "stl_util.h"


Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	destroy();
}

void Mesh::init_resource(VulkanContext& ctx, MeshData& mesh_data)
{
	m_ctx = &ctx;
	// vertex buffer
	std::vector<Vertex>& vertices = mesh_data.ref_vertices();
	auto vertex_buffer_and_memory = ctx.create_vertex_buffer((uint8_t*)vertices.data(), stlutil::vector_buffer_size(vertices));
	m_vertex_buffer.buffer = vertex_buffer_and_memory.first;
	m_vertex_buffer.memory = vertex_buffer_and_memory.second;
	// index buffer
	std::vector<uint32_t>& indices = mesh_data.ref_indices();
	auto index_buffer_and_memory = ctx.create_index_buffer((uint8_t*)indices.data(), stlutil::vector_buffer_size(indices));
	m_index_buffer.buffer = index_buffer_and_memory.first;
	m_index_buffer.memory = index_buffer_and_memory.second;
	// info
	m_index_count = indices.size();
}

void Mesh::draw(VkCommandBuffer command_buffer)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_vertex_buffer.buffer, &offset);
	vkCmdBindIndexBuffer(command_buffer, m_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(command_buffer, m_index_count, 1, 0, 0, 0);
}

void Mesh::destroy()
{
	VulkanContext& ctx = *m_ctx;
	ctx.destroy_vulkan_buffer(m_vertex_buffer);
	ctx.destroy_vulkan_buffer(m_index_buffer);
}
