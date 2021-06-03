#pragma once

#include "vulkan_context.h"
#include "image_util.h"


class TextureCubemap {
public:
	~TextureCubemap();

	void init(VulkanContext& ctx, std::vector<imageutil::Image>& raw_images);
	VkImageView get_image_view();
	VkSampler get_sampler();

	void destroy();

private:
	VulkanContext* m_ctx;
	VulkanImage m_image;
	VkImageView m_image_view;
	VkSampler m_sampler;
};
