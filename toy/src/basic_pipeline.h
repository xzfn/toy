#pragma once

#include <volk.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "vulkan_context.h"
#include "pipeline_util.h"
#include "light_manager.h"
#include "shadow_config.h"


struct FrameUniforms {
	glm::mat4 view;
	glm::mat4 view_projection;

	glm::vec3 camera_position;
	float padding_0;

	glm::vec4 sun_light_color;

	glm::vec3 sun_light_direction;
	float padding_1;

	glm::vec4 sun_cascade_splits;

	float screen_width;
	float screen_height;
	float padding_2;
	float padding_3;
};



struct ModelUniforms {
	glm::mat4 model;
};

struct MaterialUniforms {
	glm::vec3 base_color;
	float padding_0;
};

struct ShadowLayer {
	glm::mat4 light_matrix;
};

struct ShadowUniforms {
	ShadowLayer layers[MAX_SHADOWS];
};

struct DescriptorSetLayouts {
	VkDescriptorSetLayout frame;
	VkDescriptorSetLayout light;
	VkDescriptorSetLayout shadow;
	VkDescriptorSetLayout model;
	VkDescriptorSetLayout material;
};

class BasicPipeline {
public:
	BasicPipeline();
	BasicPipeline(const BasicPipeline&) = delete;
	BasicPipeline& operator=(const BasicPipeline&) = delete;

	~BasicPipeline();

	void init_pipeline(VulkanContext& ctx, PipelineDescription desc);
	std::vector<std::string> get_shader_spvs();

	void update_descriptor_set_frame(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size);
	void update_descriptor_set_light(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size);
	void update_descriptor_set_shadow(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size, VkSampler sampler, VkImageView image_view);
	void update_descriptor_set_model(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size);
	void update_descriptor_set_material(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size,
		VkSampler sampler, VkImageView image_view
	);


	void push_constants_matrix(VkCommandBuffer command_buffer, glm::mat4 matrix);

	VkPipeline get_pipeline();
	VkPipelineLayout get_pipeline_layout();
	DescriptorSetLayouts& ref_descriptor_set_layouts();

	void reload_shader();
	void destroy();

	void bind(VkCommandBuffer command_buffer);
	// NOTE MUST first n sets sequencially
	void bind_descriptor_sets(VkCommandBuffer command_buffer, std::vector<VkDescriptorSet>& descriptor_sets);

private:
	void _init_vk_pipeline();
	void _destroy_vk_pipeline();

private:
	VulkanContext* m_ctx;
	PipelineDescription m_desc;
	DescriptorSetLayouts m_descriptor_set_layouts;
	VkPipelineLayout m_pipeline_layout;
	VkPipeline m_pipeline;
};
