#pragma once

#include <volk.h>

struct VulkanBuffer {
	VkBuffer buffer;
	VkDeviceMemory memory;
};


struct VulkanImage {
	VkImage image;
	VkDeviceMemory memory;
};
