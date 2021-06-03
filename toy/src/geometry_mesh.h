#pragma once


#include "vulkan_context.h"
#include "geometry_builder.h"


class GeometryMesh {
public:
	GeometryMesh();
	~GeometryMesh();

	void init_resource(VulkanContext& ctx, std::vector<ColorLineData> data);
	void draw(VkCommandBuffer command_buffer);
	void destroy();

private:
	VulkanContext* m_ctx;
	VulkanBuffer m_vertex_buffer{ 0 };
	std::size_t m_vertex_count;
};
