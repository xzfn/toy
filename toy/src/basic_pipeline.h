#pragma once

#include <volk.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "vulkan_context.h"
#include "pipeline_util.h"


struct FrameUniforms {
	glm::mat4 view_projection;

	glm::vec3 camera_position;
	float padding_0;

	glm::vec4 sun_light_color;

	glm::vec3 sun_light_direction;
	float padding_1;
};

struct ModelUniforms {
	glm::mat4 model;
};

struct MaterialUniforms {
	glm::vec3 base_color;
	float padding_0;
};

struct DescriptorSetLayouts {
	VkDescriptorSetLayout frame;
	VkDescriptorSetLayout model;
	VkDescriptorSetLayout material;
};

class BasicPipeline {
public:
	~BasicPipeline();

	void init_pipeline(VulkanContext& ctx, PipelineDescription desc);

	void update_descriptor_set_frame(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size);
	void update_descriptor_set_model(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size);
	void update_descriptor_set_material(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size,
		VkSampler sampler, VkImageView image_view
	);
	VkPipeline get_pipeline();
	VkPipelineLayout get_pipeline_layout();
	DescriptorSetLayouts& ref_descriptor_set_layouts();

	void destroy();

private:
	VulkanContext* m_ctx;
	DescriptorSetLayouts m_descriptor_set_layouts;
	VkPipelineLayout m_pipeline_layout;
	VkPipeline m_pipeline;
};
