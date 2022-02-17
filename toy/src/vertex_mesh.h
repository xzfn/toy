#pragma once


#include "vulkan_context.h"
#include "text_builder.h"


class VertexMesh {
public:
	VertexMesh();
	VertexMesh(const VertexMesh&) = delete;
	VertexMesh& operator=(const VertexMesh&) = delete;
	~VertexMesh();

	void init_resource(VulkanContext& ctx, std::vector<ColorTextureTriangleData> data);
	void draw(VkCommandBuffer command_buffer);
	void destroy();

private:
	VulkanContext* m_ctx;
	bool m_valid{ false };
	VulkanBuffer m_vertex_buffer{ 0 };
	std::size_t m_vertex_count{ 0 };
};
