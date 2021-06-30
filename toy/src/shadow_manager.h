#pragma once

#include <array>

#include "vulkan_context.h"

#include "light_manager.h"

#include "basic_pipeline.h"

#include "shadow_config.h"


class RenderManager;


constexpr int SHADOW_DEPTH_WIDTH = 512;
constexpr int SHADOW_DEPTH_HEIGHT = 512;


class ShadowManager {
public:
	ShadowManager();
	ShadowManager(const ShadowManager&) = delete;
	ShadowManager& operator=(const ShadowManager&) = delete;

	void init(VulkanContext& ctx);
	void render_depth_pass(VkCommandBuffer command_buffer, BasicPipeline& pipeline_depth, LightManager& light_manager, RenderManager& render_manager);
	void destroy();

	VkSampler get_depth_sampler();
	VkImageView get_depth_image_view_array();
	ShadowUniforms build_shadow_uniform();

private:
	void render_one_depth(VkCommandBuffer command_buffer, BasicPipeline& pipeline_depth, RenderManager& render_manager, VkFramebuffer framebuffer, glm::mat4 view_projection);

public:
	VulkanContext* m_ctx;
	VkSampler m_depth_sampler;
	VkImage m_depth_image;  // 2d array
	VkDeviceMemory m_depth_image_memory;
	VkImageView m_depth_image_view_array;  // view for 2d array
	std::array<VkImageView, MAX_SHADOWS> m_depth_image_views;  // views for each array layer
	std::array<VkFramebuffer, MAX_SHADOWS> m_depth_framebuffers;

	VkRect2D m_depth_render_area{{0, 0}, {SHADOW_DEPTH_WIDTH, SHADOW_DEPTH_HEIGHT}};
	VkClearDepthStencilValue m_depth_clear_value{1.0f, 0};

	ShadowUniforms m_shadow_uniform;
};
