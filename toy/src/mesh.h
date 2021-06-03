#pragma once


#include "mesh_data.h"
#include "vulkan_context.h"



class Mesh {
public:
	Mesh();
	~Mesh();

	void init_resource(VulkanContext& ctx, MeshData& mesh_data);
	void draw(VkCommandBuffer command_buffer);
	void destroy();

private:
	VulkanContext* m_ctx;
	VulkanBuffer m_vertex_buffer{ 0 };
	VulkanBuffer m_index_buffer{ 0 };
	std::size_t m_index_count{ 0 };
};
