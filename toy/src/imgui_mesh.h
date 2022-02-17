#pragma once

#include <vector>
#include <string>

#include "vulkan_context.h"


class ImGuiMesh {
public:
	ImGuiMesh();
	ImGuiMesh(const ImGuiMesh&) = delete;
	ImGuiMesh& operator=(const ImGuiMesh&) = delete;
	~ImGuiMesh();

	void init_resource(VulkanContext& ctx, std::string vertex_data, std::string index_data);

	void bind(VkCommandBuffer command_buffer);
	void destroy();

private:
	VulkanContext* m_ctx;
	bool m_valid{ false };
	VulkanBuffer m_vertex_buffer{ 0 };
	VulkanBuffer m_index_buffer{ 0 };
};
