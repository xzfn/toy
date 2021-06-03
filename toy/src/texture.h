#pragma once

#include "vulkan_context.h"
#include "image_util.h"


class Texture {
public:
	~Texture();

	void init(VulkanContext& ctx, imageutil::Image& raw_image);
	VkImageView get_image_view();
	VkSampler get_sampler();

	void destroy();

private:
	VulkanContext* m_ctx;
	VulkanImage m_image;
	VkImageView m_image_view;
	VkSampler m_sampler;
};
