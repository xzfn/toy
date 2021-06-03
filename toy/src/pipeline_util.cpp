#include "pipeline_util.h"

VkPipelineInputAssemblyStateCreateInfo generate_input_assembly_state(VkPrimitiveTopology topology)
{
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        topology,  // topology;
        VK_FALSE  // primitiveRestartEnable;
    };
    return input_assembly_state;
}
