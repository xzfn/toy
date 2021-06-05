#include "vertex_mesh.h"
#include "stl_util.h"


VertexMesh::VertexMesh()
{
}

VertexMesh::~VertexMesh()
{
	destroy();
}

void VertexMesh::init_resource(VulkanContext& ctx, std::vector<ColorTextureTriangleData> data)
{
	m_ctx = &ctx;
	// vertex buffer
	auto vertex_buffer_and_memory = ctx.create_vertex_buffer((uint8_t*)data.data(), stlutil::vector_buffer_size(data));
	m_vertex_buffer.buffer = vertex_buffer_and_memory.first;
	m_vertex_buffer.memory = vertex_buffer_and_memory.second;
	m_vertex_count = data.size() * 3;
}

void VertexMesh::draw(VkCommandBuffer command_buffer)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_vertex_buffer.buffer, &offset);

	vkCmdDraw(command_buffer, m_vertex_count, 1, 0, 0);
}

void VertexMesh::destroy()
{
	VulkanContext& ctx = *m_ctx;
	ctx.destroy_vulkan_buffer(m_vertex_buffer);
}
