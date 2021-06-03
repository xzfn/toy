#include "vertex_format.h"

#include <vector>


// PositionNormalUv

std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions_position_normal_uv{
	{
		0,  // binding;
		sizeof(VertexPositionNormalUv),  // stride;
		VK_VERTEX_INPUT_RATE_VERTEX  // inputRate;
	}
};

std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions_position_normal_uv = {
    {
        0,  // location;
        vertex_binding_descriptions_position_normal_uv[0].binding,  // binding;
        VK_FORMAT_R32G32B32_SFLOAT,  // format;
        0  // offset;
    },
    {
        1,  // location;
        vertex_binding_descriptions_position_normal_uv[0].binding,  // binding;
        VK_FORMAT_R32G32B32_SFLOAT,  // format;
        sizeof(float) * 3  // offset;
    },
    {
        2,  // location;
        vertex_binding_descriptions_position_normal_uv[0].binding,  // binding;
        VK_FORMAT_R32G32_SFLOAT,  // format;
        sizeof(float) * (3 + 3)  // offset;
    }
};

VkPipelineVertexInputStateCreateInfo vertex_input_state_position_normal_uv = {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        (uint32_t)vertex_binding_descriptions_position_normal_uv.size(),  // vertexBindingDescriptionCount;
        vertex_binding_descriptions_position_normal_uv.data(),  // pVertexBindingDescriptions;
        (uint32_t)vertex_attribute_descriptions_position_normal_uv.size(),  // vertexAttributeDescriptionCount;
        vertex_attribute_descriptions_position_normal_uv.data()  // pVertexAttributeDescriptions;
};


// PositionColor

std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions_position_color{
    {
        0,  // binding;
        sizeof(VertexPositionColor),  // stride;
        VK_VERTEX_INPUT_RATE_VERTEX  // inputRate;
    }
};

std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions_position_color = {
    {
        0,  // location;
        vertex_binding_descriptions_position_color[0].binding,  // binding;
        VK_FORMAT_R32G32B32_SFLOAT,  // format;
        0  // offset;
    },
    {
        1,  // location;
        vertex_binding_descriptions_position_color[0].binding,  // binding;
        VK_FORMAT_R32G32B32_SFLOAT,  // format;
        sizeof(float) * 3  // offset;
    }
};

VkPipelineVertexInputStateCreateInfo vertex_input_state_position_color = {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        (uint32_t)vertex_binding_descriptions_position_color.size(),  // vertexBindingDescriptionCount;
        vertex_binding_descriptions_position_color.data(),  // pVertexBindingDescriptions;
        (uint32_t)vertex_attribute_descriptions_position_color.size(),  // vertexAttributeDescriptionCount;
        vertex_attribute_descriptions_position_color.data()  // pVertexAttributeDescriptions;
};



VkPipelineVertexInputStateCreateInfo generate_vertex_input_state(VertexFormat vertex_format)
{
	switch (vertex_format) {
	case VertexFormat::PositionNormalUv:
        return vertex_input_state_position_normal_uv;
    case VertexFormat::PositionColor:
        return vertex_input_state_position_color;
    default:
        break;
	}
    assert(false);
    return VkPipelineVertexInputStateCreateInfo{ };
}
