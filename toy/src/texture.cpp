#include "texture.h"

Texture::~Texture()
{
	destroy();
}

void Texture::init(VulkanContext& ctx, imageutil::Image& raw_image)
{
	m_ctx = &ctx;
	auto image_and_memory = ctx.create_texture(raw_image.width, raw_image.height, raw_image.data.data(), raw_image.data.size());
	m_image.image = image_and_memory.first;
	m_image.memory = image_and_memory.second;
	m_image_view = ctx.create_image_view_texture(m_image.image);

	m_sampler = ctx.create_sampler();
}

VkImageView Texture::get_image_view()
{
	return m_image_view;
}

VkSampler Texture::get_sampler()
{
	return m_sampler;
}

void Texture::destroy()
{
	VulkanContext& ctx = *m_ctx;
	ctx.destroy_vulkan_image(m_image);
	ctx.destroy_image_view(m_image_view);
	ctx.destroy_sampler(m_sampler);
}
