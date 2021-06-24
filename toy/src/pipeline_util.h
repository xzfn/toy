#pragma once

#include <string>

#include <volk.h>

#include "vertex_format.h"

struct PipelineDescription {
	std::string filename_vert_spv;
	std::string filename_frag_spv;
	VertexFormat vertex_format;
	VkPrimitiveTopology topology;
	VkCullModeFlags cull_model_flags;
	VkRenderPass render_pass;
};


VkPipelineInputAssemblyStateCreateInfo generate_input_assembly_state(VkPrimitiveTopology topology);

