#include "imgui_mesh.h"

ImGuiMesh::ImGuiMesh()
{
}

ImGuiMesh::~ImGuiMesh()
{
	destroy();
}

void ImGuiMesh::init_resource(VulkanContext& ctx, std::string vertex_data, std::string index_data)
{
	m_ctx = &ctx;
	// vertex buffer
	auto vertex_buffer_and_memory = ctx.create_vertex_buffer((uint8_t*)vertex_data.data(), vertex_data.size());
	m_vertex_buffer.buffer = vertex_buffer_and_memory.first;
	m_vertex_buffer.memory = vertex_buffer_and_memory.second;
	// index buffer
	auto index_buffer_and_memory = ctx.create_index_buffer((uint8_t*)index_data.data(), index_data.size());
	m_index_buffer.buffer = index_buffer_and_memory.first;
	m_index_buffer.memory = index_buffer_and_memory.second;

	m_valid = true;
}

void ImGuiMesh::bind(VkCommandBuffer command_buffer)
{
	if (!m_valid) {
		return;
	}
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(command_buffer, 0, 1, &m_vertex_buffer.buffer, &offset);
	vkCmdBindIndexBuffer(command_buffer, m_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT32);
}

void ImGuiMesh::destroy()
{
	if (!m_valid) {
		return;
	}
	VulkanContext& ctx = *m_ctx;
	ctx.destroy_vulkan_buffer(m_vertex_buffer);
	ctx.destroy_vulkan_buffer(m_index_buffer);
}
