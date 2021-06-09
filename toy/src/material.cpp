#include "material.h"
#include "texture.h"
#include "texture_cubemap.h"

Material::Material()
{
}

Material::~Material()
{
	destroy();
}

void Material::init(VulkanContext& ctx, BasicPipeline& pipeline, Texture& texture)
{
	m_ctx = &ctx;
	m_pipeline = &pipeline;
	m_texture = &texture;

	auto pipeline_descriptor_set_layouts = pipeline.ref_descriptor_set_layouts();
	VkDescriptorSet descriptor_set_material = ctx.create_descriptor_set(pipeline_descriptor_set_layouts.material);
	auto material_uniform_buffer_and_memory = ctx.create_uniform_buffer_coherent(
		nullptr, sizeof(MaterialUniforms));
	VkBuffer material_uniform_buffer = material_uniform_buffer_and_memory.first;
	VkDeviceMemory material_uniform_memory = material_uniform_buffer_and_memory.second;
	pipeline.update_descriptor_set_material(descriptor_set_material, material_uniform_buffer, sizeof(MaterialUniforms),
		texture.get_sampler(), texture.get_image_view());

	m_buffer.buffer = material_uniform_buffer;
	m_buffer.memory = material_uniform_memory;
	m_descriptor_set = descriptor_set_material;
}

void Material::init(VulkanContext& ctx, BasicPipeline& pipeline, TextureCubemap& texture)
{
	m_ctx = &ctx;
	m_pipeline = &pipeline;
	m_texture_cubemap = &texture;

	auto pipeline_descriptor_set_layouts = pipeline.ref_descriptor_set_layouts();
	VkDescriptorSet descriptor_set_material = ctx.create_descriptor_set(pipeline_descriptor_set_layouts.material);
	auto material_uniform_buffer_and_memory = ctx.create_uniform_buffer_coherent(
		nullptr, sizeof(MaterialUniforms));
	VkBuffer material_uniform_buffer = material_uniform_buffer_and_memory.first;
	VkDeviceMemory material_uniform_memory = material_uniform_buffer_and_memory.second;
	pipeline.update_descriptor_set_material(descriptor_set_material, material_uniform_buffer, sizeof(MaterialUniforms),
		texture.get_sampler(), texture.get_image_view());

	m_buffer.buffer = material_uniform_buffer;
	m_buffer.memory = material_uniform_memory;
	m_descriptor_set = descriptor_set_material;
}

void Material::bind(VkCommandBuffer command_buffer)
{
	VulkanContext& ctx = *m_ctx;
	void* memory_pointer;
	vkMapMemory(ctx.basic.device, m_buffer.memory, 0,
		sizeof(MaterialUniforms), 0, &memory_pointer);
	memcpy(memory_pointer, &m_material_uniforms, sizeof(MaterialUniforms));
	vkUnmapMemory(ctx.basic.device, m_buffer.memory);

	uint32_t first_set = 2;
	vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get_pipeline_layout(),
		2, 1, &m_descriptor_set, 0, nullptr);
}

BasicPipeline* Material::get_pipeline()
{
	return m_pipeline;
}

MaterialUniforms& Material::ref_uniforms()
{
	return m_material_uniforms;
}

void Material::destroy()
{
	VulkanContext& ctx = *m_ctx;
	std::vector<VkDescriptorSet> descriptor_sets{ m_descriptor_set };
	//ctx.free_descriptor_sets(descriptor_sets);
	ctx.destroy_vulkan_buffer(m_buffer);
}
