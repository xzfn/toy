#include "geometry_mesh.h"

#include "stl_util.h"


GeometryMesh::GeometryMesh()
{
}

GeometryMesh::~GeometryMesh()
{
	destroy();
}

void GeometryMesh::init_resource(VulkanContext& ctx, std::vector<ColorLineData> data)
{
	if (data.empty()) {
		return;
	}

	m_ctx = &ctx;
	// vertex buffer
	auto vertex_buffer_and_memory = ctx.create_vertex_buffer((uint8_t*)data.data(), stlutil::vector_buffer_size(data));
	m_vertex_buffer.buffer = vertex_buffer_and_memory.first;
	m_vertex_buffer.memory = vertex_buffer_and_memory.second;
	m_vertex_count = data.size() * 2;
	m_valid = true;
}

void GeometryMesh::init_resource(VulkanContext& ctx, GeometryMeshData data)
{
	init_resource(ctx, data.data);
}

void GeometryMesh::draw(VkCommandBuffer command_buffer)
{
	if (!m_valid) {
		return;
	}
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_vertex_buffer.buffer, &offset);

	vkCmdDraw(command_buffer, m_vertex_count, 1, 0, 0);
}

void GeometryMesh::destroy()
{
	if (!m_valid) {
		return;
	}
	m_ctx->destroy_vulkan_buffer(m_vertex_buffer);
	m_valid = false;
}
