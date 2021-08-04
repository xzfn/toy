#pragma once


#include "mesh_data.h"
#include "vulkan_context.h"
#include "bounding_util.h"


class Mesh {
public:
	Mesh();
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	~Mesh();

	void init_resource(VulkanContext& ctx, MeshData& mesh_data);
	void draw(VkCommandBuffer command_buffer);
	void destroy();

	AABB get_bounding_box();

private:
	VulkanContext* m_ctx;
	VulkanBuffer m_vertex_buffer{ 0 };
	VulkanBuffer m_index_buffer{ 0 };
	std::size_t m_index_count{ 0 };
	AABB m_bounding_box;
};
