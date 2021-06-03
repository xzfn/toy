#include "texture_cubemap.h"


TextureCubemap::~TextureCubemap()
{
	destroy();
}

void TextureCubemap::init(VulkanContext& ctx, std::vector<imageutil::Image>& raw_images)
{
	m_ctx = &ctx;
	assert(raw_images.size() == 6);
	uint32_t width = raw_images[0].width;
	uint32_t height = raw_images[0].height;
	std::vector<uint8_t> data;
	for (std::size_t i = 0; i < 6; ++i) {
		imageutil::Image& image = raw_images[i];
		data.insert(data.end(), image.data.begin(), image.data.end());
	}
	
	auto image_and_memory = ctx.create_texture_cubemap(width, height, data.data(), data.size());
	m_image.image = image_and_memory.first;
	m_image.memory = image_and_memory.second;
	m_image_view = ctx.create_image_view_texture_cubemap(m_image.image);

	m_sampler = ctx.create_sampler();
}

VkImageView TextureCubemap::get_image_view()
{
	return m_image_view;
}

VkSampler TextureCubemap::get_sampler()
{
	return m_sampler;
}

void TextureCubemap::destroy()
{
	VulkanContext& ctx = *m_ctx;
	ctx.destroy_vulkan_image(m_image);
	ctx.destroy_image_view(m_image_view);
	ctx.destroy_sampler(m_sampler);
}
