#include "vulkan_helper.h"

namespace vkhelper {

void flip_viewport(VkViewport& viewport)
{
    viewport.y = viewport.height;
    viewport.height = -viewport.height;
}

} // namespace vkhelper
