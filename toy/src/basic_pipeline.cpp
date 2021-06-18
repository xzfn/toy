#include "basic_pipeline.h"

#include <glm/ext.hpp>

#include "vulkan_util.h"
#include "vertex_format.h"


VkDescriptorSetLayout create_descriptor_set_layout_frame(VkDevice device) {
	std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
        {
            0,  // binding;
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // descriptorType;
            1,  // descriptorCount;
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,  // stageFlags;
            nullptr  // pImmutableSamplers;
        }
	};
    return vkutil::create_descriptor_set_layout(device, layout_bindings);
}

VkDescriptorSetLayout create_descriptor_set_layout_model(VkDevice device) {
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
        {
            0,  // binding;
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // descriptorType;
            1,  // descriptorCount;
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,  // stageFlags;
            nullptr  // pImmutableSamplers;
        }
    };
    return vkutil::create_descriptor_set_layout(device, layout_bindings);
}

VkDescriptorSetLayout create_descriptor_set_layout_material(VkDevice device) {
    std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
        {
            0,  // binding;
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // descriptorType;
            1,  // descriptorCount;
            VK_SHADER_STAGE_FRAGMENT_BIT,  // stageFlags;
            nullptr  // pImmutableSamplers;
        },
        {
            1,  // binding;
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // descriptorType;
            1,  // descriptorCount;
            VK_SHADER_STAGE_FRAGMENT_BIT,  // stageFlags;
            nullptr  // pImmutableSamplers;
        }
    };
    return vkutil::create_descriptor_set_layout(device, layout_bindings);
}

BasicPipeline::BasicPipeline()
{
}

void BasicPipeline::init_pipeline(VulkanContext& ctx, PipelineDescription desc)
{
    m_ctx = &ctx;
    VkDevice device = ctx.basic.device;
    m_desc = desc;
    m_descriptor_set_layouts.frame = create_descriptor_set_layout_frame(device);
    m_descriptor_set_layouts.model = create_descriptor_set_layout_model(device);
    m_descriptor_set_layouts.material = create_descriptor_set_layout_material(device);
    std::vector<VkDescriptorSetLayout> descriptor_set_layouts = {
        m_descriptor_set_layouts.frame,
        m_descriptor_set_layouts.model,
        m_descriptor_set_layouts.material
    };
    std::vector<VkPushConstantRange> push_constant_ranges = {
        {
            VK_SHADER_STAGE_VERTEX_BIT,  // stageFlags;
            0,  // offset;
            sizeof(glm::mat4)  // size;
        }
    };
    m_pipeline_layout = vkutil::create_pipeline_layout(device, descriptor_set_layouts, push_constant_ranges);

    _init_vk_pipeline();
}

BasicPipeline::~BasicPipeline()
{
    destroy();
}

void BasicPipeline::reload_shader()
{
    m_ctx->device_wait_idle();
    _destroy_vk_pipeline();
    _init_vk_pipeline();
}

void BasicPipeline::destroy()
{
    VkDevice device = m_ctx->basic.device;
    vkutil::destroy_descriptor_set_layout(device, m_descriptor_set_layouts.frame);
    vkutil::destroy_descriptor_set_layout(device, m_descriptor_set_layouts.model);
    vkutil::destroy_descriptor_set_layout(device, m_descriptor_set_layouts.material);
    vkutil::destroy_pipeline_layout(device, m_pipeline_layout);
    _destroy_vk_pipeline();
}

void BasicPipeline::bind(VkCommandBuffer command_buffer)
{
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, get_pipeline());
}

void BasicPipeline::_init_vk_pipeline()
{
    VulkanContext& ctx = *m_ctx;
    VkDevice device = ctx.basic.device;
    auto& desc = m_desc;

    m_pipeline = vkutil::create_pipeline(device, ctx.basic.render_pass, ctx.basic.extent.width, ctx.basic.extent.height, desc.filename_vert_spv, desc.filename_frag_spv,
        generate_vertex_input_state(desc.vertex_format),
        generate_input_assembly_state(desc.topology),
        m_pipeline_layout, desc.cull_model_flags);
}

void BasicPipeline::_destroy_vk_pipeline()
{
    VulkanContext& ctx = *m_ctx;
    VkDevice device = ctx.basic.device;
    vkutil::destroy_pipeline(device, m_pipeline);
}

void BasicPipeline::update_descriptor_set_frame(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size)
{
    VkDescriptorBufferInfo uniform_buffer_info = {
        uniform_buffer,  // buffer;
        0,  // offset;
        uniform_buffer_size  // range;
    };
    VkWriteDescriptorSet descriptor_write = {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,  // sType;
        nullptr,  // pNext;
        descriptor_set,  // dstSet;
        0,  // dstBinding;
        0,  // dstArrayElement;
        1,  // descriptorCount;
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // descriptorType;
        nullptr,  // pImageInfo;
        &uniform_buffer_info,  // pBufferInfo;
        nullptr  // pTexelBufferView;
    };
    auto& ctx = *m_ctx;
    vkUpdateDescriptorSets(ctx.basic.device, 1, &descriptor_write, 0, nullptr);
}

void BasicPipeline::update_descriptor_set_model(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size)
{
    VkDescriptorBufferInfo uniform_buffer_info = {
        uniform_buffer,  // buffer;
        0,  // offset;
        uniform_buffer_size  // range;
    };
     VkWriteDescriptorSet descriptor_write = {
         VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,  // sType;
         nullptr,  // pNext;
         descriptor_set,  // dstSet;
         0,  // dstBinding;
         0,  // dstArrayElement;
         1,  // descriptorCount;
         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // descriptorType;
         nullptr,  // pImageInfo;
         &uniform_buffer_info,  // pBufferInfo;
         nullptr  // pTexelBufferView;
    };
    auto& ctx = *m_ctx;
    vkUpdateDescriptorSets(ctx.basic.device, 1, &descriptor_write, 0, nullptr);
}

void BasicPipeline::update_descriptor_set_material(VkDescriptorSet descriptor_set, VkBuffer uniform_buffer, std::size_t uniform_buffer_size,
    VkSampler sampler, VkImageView image_view)
{
    VkDescriptorBufferInfo uniform_buffer_info = {
        uniform_buffer,  // buffer;
        0,  // offset;
        uniform_buffer_size  // range;
    };
    VkDescriptorImageInfo image_info = {
        sampler,  // sampler;
        image_view,  // imageView;
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  // imageLayout;
    };
    std::vector<VkWriteDescriptorSet> descriptor_writes = {
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,  // sType;
            nullptr,  // pNext;
            descriptor_set,  // dstSet;
            0,  // dstBinding;
            0,  // dstArrayElement;
            1,  // descriptorCount;
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // descriptorType;
            nullptr,  // pImageInfo;
            &uniform_buffer_info,  // pBufferInfo;
            nullptr  // pTexelBufferView;
        },
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,  // sType;
            nullptr,  // pNext;
            descriptor_set,  // dstSet;
            1,  // dstBinding;
            0,  // dstArrayElement;
            1,  // descriptorCount;
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // descriptorType;
            &image_info,  // pImageInfo;
            nullptr,  // pBufferInfo;
            nullptr  // pTexelBufferView;
        }
    };
    auto& ctx = *m_ctx;
    vkUpdateDescriptorSets(ctx.basic.device, descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
}

void BasicPipeline::push_constants_matrix(VkCommandBuffer command_buffer, glm::mat4 matrix)
{
    vkCmdPushConstants(command_buffer, get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(matrix), glm::value_ptr(matrix));
}

void update_descriptor_set_textures(VkDescriptorSet descriptor_set, VkSampler sampler, VkImageView image_view)
{
    VkDescriptorImageInfo image_info = {
        sampler,  // sampler;
        image_view,  // imageView;
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  // imageLayout;
    };
    VkWriteDescriptorSet descriptor_write = {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,  // sType;
        nullptr,  // pNext;
        descriptor_set,  // dstSet;
        0,  // dstBinding;
        0,  // dstArrayElement;
        1,  // descriptorCount;
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // descriptorType;
        & image_info,  // pImageInfo;
        nullptr,  // pBufferInfo;
        nullptr  // pTexelBufferView;
    };
    //auto& ctx = *m_ctx;
    //vkUpdateDescriptorSets(ctx.basic.device, 1, &descriptor_write, 0, nullptr);
}

VkPipeline BasicPipeline::get_pipeline()
{
    return m_pipeline;
}

VkPipelineLayout BasicPipeline::get_pipeline_layout()
{
    return m_pipeline_layout;
}

DescriptorSetLayouts& BasicPipeline::ref_descriptor_set_layouts()
{
    return m_descriptor_set_layouts;
}

