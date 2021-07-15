#include "shadow_manager.h"

#include <glm/ext.hpp>

#include "glm_util.h"

#include "vulkan_helper.h"
#include "render_manager.h"


ShadowManager::ShadowManager()
{
}

void ShadowManager::init(VulkanContext& ctx)
{
	m_ctx = &ctx;

	m_depth_sampler = ctx.create_depth_sampler();

	auto image_and_memory = ctx.create_texture_depth(SHADOW_DEPTH_WIDTH, SHADOW_DEPTH_HEIGHT, MAX_SHADOWS, true);
	m_depth_image = image_and_memory.first;
	m_depth_image_memory = image_and_memory.second;

	m_depth_image_view_array = ctx.create_image_view_texture_depth(m_depth_image, true, false, 0, MAX_SHADOWS);

	for (int i = 0; i < MAX_SHADOWS; ++i) {
		VkImageView depth_image_view = ctx.create_image_view_texture_depth(m_depth_image, true, false, i, 1);
		m_depth_image_views[i] = depth_image_view;
		std::vector<VkImageView> framebuffer_image_views{ depth_image_view };
		m_depth_framebuffers[i] = ctx.create_framebuffer(SHADOW_DEPTH_WIDTH, SHADOW_DEPTH_HEIGHT, ctx.basic.depth_render_pass, framebuffer_image_views);
	}
}

void ShadowManager::render_depth_pass(VkCommandBuffer command_buffer, BasicPipeline& pipeline_depth, LightManager& light_manager, RenderManager& render_manager)
{
	glm::mat4 light_view_projections[MAX_SHADOWS];

	int current_layer_index = 0;

	// sun
	auto psun = light_manager.get_sun();
	if (psun) {
		Light& sun = *psun;
		glm::mat4 projection = glm::orthoRH_ZO(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f);
		Transform light_model = Transform();
		light_model.rotation = look_rotation_to_quat(sun.get_direction(), VEC3_Y);
		glm::mat4 light_view = glm::inverse(transform_to_mat4(light_model));
		glm::mat4 sun_light_view_projection = projection * light_view;
		light_view_projections[current_layer_index] = sun_light_view_projection;
		++current_layer_index;
	}

	// regular lights, spot only
	for (auto& plight : light_manager.ref_lights()) {
		Light& light = *plight;
		if (light.get_shadow()) {
			if (current_layer_index >= MAX_SHADOWS) {
				light.internal_set_shadow_layer(-1);
			}
			else if (light.get_type() == LightType::Spot) {
				Transform light_model;
				glm::vec3 spot_position = light.get_position();
				glm::vec3 spot_direction = light.get_direction();
				light_model.translation = spot_position;
				glm::vec3 up = VEC3_Y;
				if (abs(glm::dot(spot_direction, up)) > 0.9) {
					up = VEC3_Z;
				}
				light_model.rotation = look_rotation_to_quat(spot_direction, up);
				glm::mat4 light_view = glm::inverse(transform_to_mat4(light_model));
				float spot_outer_angle = light.get_spot_outer_angle();
				glm::mat4 projection = glm::perspective(spot_outer_angle, 1.0f, 0.1f, 20.0f);
				glm::mat4 spot_light_view_projection = projection * light_view;
				light_view_projections[current_layer_index] = spot_light_view_projection;
				// set current_layer_index to light uniform
				light.internal_set_shadow_layer(current_layer_index);
				++current_layer_index;
			}
		}
	}
	int shadow_count = current_layer_index;

	// render depth
	for (int shadow_index = 0; shadow_index < shadow_count; ++shadow_index) {
		VkFramebuffer framebuffer = m_depth_framebuffers[shadow_index];
		glm::mat4 light_view_projection = light_view_projections[shadow_index];
		// write shadow uniform, to be used later
		m_shadow_uniform.layers[shadow_index].light_matrix = light_view_projection;

		render_one_depth(command_buffer, pipeline_depth, render_manager, framebuffer, light_view_projection);
	}
}

void ShadowManager::render_one_depth(VkCommandBuffer command_buffer, BasicPipeline& pipeline_depth, RenderManager& render_manager, VkFramebuffer framebuffer, glm::mat4 view_projection)
{
	VulkanContext& ctx = *m_ctx;

	std::array<VkClearValue, 1> clear_values;
	clear_values[0].depthStencil = m_depth_clear_value;

	// begin depth render pass
	VkRenderPassBeginInfo render_pass_begin_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,  // sType;
		nullptr,  // pNext;
		ctx.basic.depth_render_pass,  // renderPass;
		framebuffer,  // framebuffer;
		m_depth_render_area,  // renderArea;
		(uint32_t)clear_values.size(),  // clearValueCount;
		clear_values.data()  // pClearValues;
	};
	vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

	// viewport and scissor
	VkViewport viewport = {
		0.0f, 0.0f, (float)SHADOW_DEPTH_WIDTH, (float)SHADOW_DEPTH_HEIGHT, 0.0f, 1.0f
	};
	vkhelper::flip_viewport(viewport);

	VkRect2D scissor = {
		{
			0, 0
		},
		{
			SHADOW_DEPTH_WIDTH, SHADOW_DEPTH_HEIGHT
		}
	};
	vkCmdSetViewport(command_buffer, 0, 1, &viewport);
	vkCmdSetScissor(command_buffer, 0, 1, &scissor);

	// frame descriptor set, only need view_projection as camera
	FrameUniforms light_frame_uniforms;
	light_frame_uniforms.view_projection = view_projection;

	VkDescriptorSet light_frame_descriptor_set = ctx.create_descriptor_set_transient(pipeline_depth.ref_descriptor_set_layouts().frame);
	auto buffer_and_memory = ctx.create_uniform_buffer_coherent((uint8_t*)&light_frame_uniforms, sizeof(light_frame_uniforms));
	pipeline_depth.update_descriptor_set_frame(light_frame_descriptor_set, buffer_and_memory.first, sizeof(light_frame_uniforms));
	ctx.destroy_vulkan_buffer(VulkanBuffer{ buffer_and_memory.first, buffer_and_memory.second });

	std::vector<VkDescriptorSet> descriptor_sets_frame{
		light_frame_descriptor_set
	};

	pipeline_depth.bind(command_buffer);
	pipeline_depth.bind_descriptor_sets(command_buffer, descriptor_sets_frame);

	// actual render
	render_manager.render_depth(command_buffer, pipeline_depth);

	// end depth render pass
	vkCmdEndRenderPass(command_buffer);
}

void ShadowManager::destroy()
{
	VulkanContext& ctx = *m_ctx;
	for (int i = 0; i < MAX_SHADOWS; ++i) {
		ctx.destroy_framebuffer(m_depth_framebuffers[i]);
		ctx.destroy_image_view(m_depth_image_views[i]);
	}
	ctx.destroy_image_view(m_depth_image_view_array);
	ctx.destroy_image(m_depth_image);
	ctx.free_memory(m_depth_image_memory);
	ctx.destroy_sampler(m_depth_sampler);
}

VkSampler ShadowManager::get_depth_sampler()
{
	return m_depth_sampler;
}

VkImageView ShadowManager::get_depth_image_view_array()
{
	return m_depth_image_view_array;
}

ShadowUniforms ShadowManager::build_shadow_uniform()
{
	return m_shadow_uniform;
}
