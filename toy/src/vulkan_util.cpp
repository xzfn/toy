#include "vulkan_util.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <cassert>

#include "file_util.h"


namespace vkutil {

VkAllocationCallbacks* vulkan_allocator = nullptr;

void check_vk_result(VkResult vkres) {
	if (vkres != VK_SUCCESS) {
		std::cout << "VkResult is not VK_SUCCESS: " << vkres << "\n";
        std::abort();
	}
}

std::string version_to_string(uint32_t version) {
    std::stringstream ss;
    ss << VK_VERSION_MAJOR(version) << '.'
        << VK_VERSION_MINOR(version) << '.'
        << VK_VERSION_PATCH(version);
    return ss.str();
}

VkInstance create_vulkan_instance() {
	VkResult vkres;

    VkApplicationInfo application_info = {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,  // sType;
        nullptr,  // pNext;
        "game",  // pApplicationName;
        VK_MAKE_VERSION(1, 0, 0),  // applicationVersion;
        "toy",  // pEngineName;
        VK_MAKE_VERSION(1, 0, 0),  // engineVersion;
        VK_API_VERSION_1_2,  // apiVersion;
    };

    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    VkInstanceCreateInfo create_info = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        &application_info,  // pApplicationInfo;
        0,  // enabledLayerCount;
        nullptr,  // ppEnabledLayerNames;
        (uint32_t)extensions.size(),  // enabledExtensionCount;
        extensions.data()  // ppEnabledExtensionNames;
    };

    VkInstance instance = VK_NULL_HANDLE;
    vkres = vkCreateInstance(&create_info, vulkan_allocator, &instance);
    check_vk_result(vkres);
    return instance;
}

std::vector<VkPhysicalDevice> get_physical_devices(VkInstance instance)
{
    VkResult vkres;
    uint32_t num_devices = 0;

    vkres = vkEnumeratePhysicalDevices(instance, &num_devices, nullptr);
    check_vk_result(vkres);

    std::vector<VkPhysicalDevice> physical_devices(num_devices);

    vkres = vkEnumeratePhysicalDevices(instance, &num_devices, physical_devices.data());
    check_vk_result(vkres);
    return physical_devices;
}

void dump_physical_device(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;

    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &device_features);

    std::cout << "PhysicalDevice " << physical_device << "\n";
    std::cout << "    deviceName " << device_properties.deviceName << "\n";
    std::cout << "    apiVersion " << version_to_string(device_properties.apiVersion) << "\n";
    std::cout << "    limits.maxImageDimension2D " << device_properties.limits.maxImageDimension2D << "\n";

}

VkPhysicalDeviceMemoryProperties get_physical_device_memory_properties(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
    return memory_properties;
}

VkSurfaceKHR create_win32_surface(VkInstance instance, HINSTANCE hinstance, HWND hwnd)
{
    VkResult vkres;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkWin32SurfaceCreateInfoKHR create_info = {
        VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        hinstance,  // hinstance;
        hwnd  // hwnd;
    };
    vkres = vkCreateWin32SurfaceKHR(instance, &create_info, vulkan_allocator, &surface);
    check_vk_result(vkres);
    return surface;
}

std::vector<VkQueueFamilyProperties> get_queue_family_properties(VkPhysicalDevice physical_device) {
    uint32_t family_property_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_property_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_family_properties(family_property_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &family_property_count, queue_family_properties.data());
    return queue_family_properties;
}

std::pair<uint32_t, uint32_t> select_graphics_present_queue_family(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    uint32_t graphics_queue_family_index = UINT32_MAX;
    uint32_t present_queue_family_index = UINT32_MAX;
    auto queue_family_properties = get_queue_family_properties(physical_device);
    uint32_t queue_family_count = (uint32_t)queue_family_properties.size();
    std::vector<VkBool32> present_supports(queue_family_count);
    for (uint32_t i = 0; i < queue_family_count; ++i) {
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_supports[i]);
        VkQueueFamilyProperties& properties = queue_family_properties[i];
        if (properties.queueCount > 0 && (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            if (graphics_queue_family_index == UINT32_MAX) {
                graphics_queue_family_index = i;
            }
            if (present_supports[i]) {
                // support both graphics and present
                graphics_queue_family_index = i;
                present_queue_family_index = i;
                break;
            }
        }
    }

    if (present_queue_family_index == UINT32_MAX) {
        // no queue supports both graphics and present, find a separate present queue
        for (uint32_t i = 0; i < queue_family_count; ++i) {
            if (present_supports[i]) {
                present_queue_family_index = i;
                break;
            }
        }
    }

    if (graphics_queue_family_index == UINT32_MAX || present_queue_family_index == UINT32_MAX) {
        // cannot find both, even different
        assert(false);
    }

    return std::make_pair(graphics_queue_family_index, present_queue_family_index);
}

VkDevice create_device(VkPhysicalDevice physical_device, uint32_t queue_family_index)
{
    VkResult vkres;

    std::vector<const char*> device_extensions{
        "VK_KHR_swapchain"
    };
    float queue_priority[] = { 1.0f };

    VkDeviceQueueCreateInfo queue_create_infos[1] = {
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,  // sType;
            nullptr,  // pNext;
            0,  // flags;
            queue_family_index,  // queueFamilyIndex;
            1,  // queueCount;
            queue_priority,  // pQueuePriorities;
        }
    };

    VkDeviceCreateInfo create_info = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        1,  // queueCreateInfoCount;
        queue_create_infos,  // pQueueCreateInfos;
        0,  // enabledLayerCount;
        nullptr,  // ppEnabledLayerNames;
        (uint32_t)device_extensions.size(),  // enabledExtensionCount;
        device_extensions.data(),  // ppEnabledExtensionNames;
        nullptr  // pEnabledFeatures;
    };

    VkDevice device;
    vkres = vkCreateDevice(physical_device, &create_info, vulkan_allocator, &device);
    check_vk_result(vkres);
    return device;
}

VkQueue get_device_queue(VkDevice device, uint32_t queue_family_index)
{
    VkQueue queue;
    vkGetDeviceQueue(device, queue_family_index, 0, &queue);
    return queue;
}

void dump_available_extensions()
{
    VkResult vkres;
    uint32_t extension_count = 0;
    vkres = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    check_vk_result(vkres);
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkres = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, available_extensions.data());
    check_vk_result(vkres);
    std::cout << "Available extensions\n";
    for (VkExtensionProperties& extension : available_extensions) {
        std::cout << "    " << extension.extensionName << "\n";
    }
}

std::vector<VkPresentModeKHR> get_present_modes(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    VkResult vkres;
    uint32_t mode_count;
    vkres = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &mode_count, nullptr);
    check_vk_result(vkres);

    std::vector<VkPresentModeKHR> present_modes(mode_count);

    vkres = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &mode_count, present_modes.data());
    check_vk_result(vkres);
    return present_modes;
}

VkPresentModeKHR select_present_mode(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    std::vector<VkPresentModeKHR> present_modes = get_present_modes(physical_device, surface);
    for (VkPresentModeKHR& present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }
    for (VkPresentModeKHR& present_mode : present_modes) {
        if (present_mode == VK_PRESENT_MODE_FIFO_KHR) {
            return present_mode;
        }
    }
    assert(false);
    return (VkPresentModeKHR)(-1);
}

std::vector<VkSurfaceFormatKHR> get_surface_formats(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    VkResult vkres;
    uint32_t format_count = 0;
    vkres = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
    check_vk_result(vkres);
    std::vector<VkSurfaceFormatKHR> surface_formats(format_count);
    vkres = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, surface_formats.data());
    check_vk_result(vkres);

    return surface_formats;
}

VkSurfaceFormatKHR select_surface_format(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    std::vector<VkSurfaceFormatKHR> surface_formats = get_surface_formats(physical_device, surface);
    if (surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED) {
        // any format is fine
        VkSurfaceFormatKHR default_format = {
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };
        return default_format;
    }
    for (VkSurfaceFormatKHR& surface_format : surface_formats) {
        if (surface_format.format == VK_FORMAT_R8G8B8A8_UNORM) {
            return surface_format;
        }
    }
    return surface_formats[0];
}

VkFormat select_depth_format(VkPhysicalDevice physical_device)
{
    std::vector<VkFormat> depth_formats{
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT,
        VK_FORMAT_D16_UNORM
    };
    for (auto& depth_format : depth_formats) {
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(physical_device, depth_format, &format_properties);
        if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return depth_format;
        }
    }
    assert(false);  // cannot find depth format, may try linearTilingFeatures
    return VK_FORMAT_UNDEFINED;
}

VkSurfaceCapabilitiesKHR get_surface_capabilities(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    VkResult vkres;
    VkSurfaceCapabilitiesKHR surface_capabilities;
    vkres = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);
    assert(vkres == VK_SUCCESS);
    return surface_capabilities;
}

uint32_t clamp_uint32(uint32_t v, uint32_t lo, uint32_t hi) {
    if (v < lo) {
        return lo;
    }
    else if (v > hi) {
        return hi;
    }
    return v;
}

VkExtent2D clamp_extent_2d(VkExtent2D hint_extent, VkExtent2D min_extent, VkExtent2D max_extent) {
    VkExtent2D extent;
    extent.width = clamp_uint32(hint_extent.width, min_extent.width, max_extent.width);
    extent.height = clamp_uint32(hint_extent.height, min_extent.height, max_extent.height);
    return extent;
}

VkExtent2D get_swapchain_extent(VkSurfaceCapabilitiesKHR surface_capabilities, VkExtent2D hint_extent) {
    VkExtent2D extent;
    if (surface_capabilities.currentExtent.width == 0xffffffff) {
        extent = clamp_extent_2d(hint_extent, surface_capabilities.minImageExtent, surface_capabilities.maxImageExtent);
    }
    else {
        extent = surface_capabilities.currentExtent;
    }
    return extent;
}

uint32_t get_min_image_count_from_present_mode(VkPresentModeKHR present_mode) {
    if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
        return 3;
    }
    if (present_mode == VK_PRESENT_MODE_FIFO_KHR || present_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
        return 2;
    }
    if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
        return 1;
    }
    assert(false);
    return 1;
}

VkImageUsageFlags get_swapchain_usage(VkSurfaceCapabilitiesKHR surface_capabilities) {
    if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }
    assert(false);
    return (VkImageUsageFlags)(-1);
}

VkSurfaceTransformFlagBitsKHR get_swapchain_transform(VkSurfaceCapabilitiesKHR surface_capabilities) {
    if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else {
        return surface_capabilities.currentTransform;
    }
}

std::pair<VkSwapchainKHR, VkExtent2D> create_swapchain(VkPhysicalDevice physical_device, VkDevice device, VkSurfaceKHR surface, VkPresentModeKHR present_mode, VkSurfaceFormatKHR surface_format, VkExtent2D hint_extent, VkSwapchainKHR old_swapchain)
{
    VkResult vkres;
    VkSwapchainKHR swapchain;
    VkSurfaceCapabilitiesKHR surface_capabilities = get_surface_capabilities(physical_device, surface);
    VkExtent2D image_extent = get_swapchain_extent(surface_capabilities, hint_extent);
    if (image_extent.width == 0 && image_extent.height == 0) {
        std::cout << "create_swapchain zero image_extent\n";
        return std::pair<VkSwapchainKHR, VkExtent2D>(VK_NULL_HANDLE, image_extent);
    }
    VkSwapchainCreateInfoKHR create_info = {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        surface,  // surface;
        get_min_image_count_from_present_mode(present_mode),  // minImageCount;
        surface_format.format,  // imageFormat;
        surface_format.colorSpace,  // imageColorSpace;
        image_extent,  // imageExtent;
        1,  // imageArrayLayers;
        get_swapchain_usage(surface_capabilities),  // imageUsage;
        VK_SHARING_MODE_EXCLUSIVE,  // imageSharingMode;
        0,  // queueFamilyIndexCount;
        nullptr,  // pQueueFamilyIndices;
        get_swapchain_transform(surface_capabilities),  // preTransform;
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,  // compositeAlpha;
        present_mode,  // presentMode;
        VK_TRUE,  // clipped;
        old_swapchain  // oldSwapchain;
    };
    vkres = vkCreateSwapchainKHR(device, &create_info, vulkan_allocator, &swapchain);
    check_vk_result(vkres);
    if (old_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, old_swapchain, vulkan_allocator);
    }

    return std::make_pair(swapchain, image_extent);
}

std::vector<VkImage> get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain)
{
    VkResult vkres;
    uint32_t image_count = 0;
    vkres = vkGetSwapchainImagesKHR(device, swapchain, &image_count, nullptr);
    assert(vkres == VK_SUCCESS);
    std::vector<VkImage> swapchain_images(image_count);
    vkres = vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());
    assert(vkres == VK_SUCCESS);
    return swapchain_images;
}

VkImageView create_image_view(VkDevice device, VkSurfaceFormatKHR surface_format, VkImage image)
{
    VkResult vkres;
    VkImageViewCreateInfo create_info = {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        image,  // image;
        VK_IMAGE_VIEW_TYPE_2D,  // viewType;
        surface_format.format,  // format;
        {
            VK_COMPONENT_SWIZZLE_R,
            VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B,
            VK_COMPONENT_SWIZZLE_A,
        },  // components;
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            1
        }  // subresourceRange;
    };
    VkImageView image_view = VK_NULL_HANDLE;
    vkres = vkCreateImageView(device, &create_info, vulkan_allocator, &image_view);
    assert(vkres == VK_SUCCESS);
    return image_view;
}

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index)
{
    VkResult vkres;
    VkCommandPoolCreateInfo create_info = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,  // flags;
        queue_family_index  // queueFamilyIndex;
    };
    VkCommandPool command_pool = VK_NULL_HANDLE;
    vkres = vkCreateCommandPool(device, &create_info, vulkan_allocator, &command_pool);
    assert(vkres == VK_SUCCESS);
    return command_pool;
}

VkCommandBuffer create_command_buffer(VkDevice device, VkCommandPool command_pool)
{
    VkResult vkres;
    VkCommandBufferAllocateInfo allocate_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,  // sType;
        nullptr,  // pNext;
        command_pool,  // commandPool;
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,  // level;
        1  // commandBufferCount;
    };
    VkCommandBuffer command_buffer;
    vkres = vkAllocateCommandBuffers(device, &allocate_info, &command_buffer);
    assert(vkres == VK_SUCCESS);
    return command_buffer;
}

VkSemaphore create_semaphore(VkDevice device)
{
    VkResult vkres;
    VkSemaphore semaphore;
    VkSemaphoreCreateInfo create_info = {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0  // flags;
    };
    vkres = vkCreateSemaphore(device, &create_info, vulkan_allocator, &semaphore);
    assert(vkres == VK_SUCCESS);
    return semaphore;
}

VkFence create_fence(VkDevice device) {
    VkResult vkres;
    VkFenceCreateInfo create_info = {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        VK_FENCE_CREATE_SIGNALED_BIT  // flags;
    };
    VkFence fence = VK_NULL_HANDLE;
    vkres = vkCreateFence(device, &create_info, vulkan_allocator, &fence);
    assert(vkres == VK_SUCCESS);
    return fence;
}

VkFramebuffer create_framebuffer(VkDevice device, uint32_t width, uint32_t height, VkRenderPass render_pass, std::vector<VkImageView>& image_views)
{
    VkResult vkres;
    VkFramebufferCreateInfo create_info = {
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        render_pass,  // renderPass;
        (uint32_t)image_views.size(),  // attachmentCount;
        image_views.data(),  // pAttachments;
        width,  // width;
        height,  // height;
        1  // layers;
    };
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    vkres = vkCreateFramebuffer(device, &create_info, vulkan_allocator, &framebuffer);
    assert(vkres == VK_SUCCESS);
    return framebuffer;
}

void destroy_framebuffer(VkDevice device, VkFramebuffer framebuffer)
{
    vkDestroyFramebuffer(device, framebuffer, vulkan_allocator);
}

VkRenderPass create_render_pass(VkDevice device, VkSurfaceFormatKHR surface_format, VkFormat depth_format)
{
    VkResult vkres;
    std::vector<VkAttachmentDescription> attachment_descriptions{
        {
            0,  // flags;
            surface_format.format,  // format;
            VK_SAMPLE_COUNT_1_BIT,  // samples;
            VK_ATTACHMENT_LOAD_OP_CLEAR,  // loadOp;
            VK_ATTACHMENT_STORE_OP_STORE,  // storeOp;
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // stencilLoadOp;
            VK_ATTACHMENT_STORE_OP_DONT_CARE,  // stencilStoreOp;
            VK_IMAGE_LAYOUT_UNDEFINED,  // initialLayout;
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  // finalLayout;
        },
        {
            0,  // flags;
            depth_format,  // format;
            VK_SAMPLE_COUNT_1_BIT,  // samples;
            VK_ATTACHMENT_LOAD_OP_CLEAR,  // loadOp;
            VK_ATTACHMENT_STORE_OP_DONT_CARE,  // storeOp;
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // stencilLoadOp;
            VK_ATTACHMENT_STORE_OP_DONT_CARE,  // stencilStoreOp;
            VK_IMAGE_LAYOUT_UNDEFINED,  // initialLayout;
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // finalLayout;
        }
    };
    VkAttachmentReference color_attachment_references[1] = {
        {
            0,  // attachment
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // layout;
        }
    };
    VkAttachmentReference depth_attachment_references[1] = {
        {
            1,  // attachment
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // layout;
        }
    };
    VkSubpassDescription subpass_descriptions[1] = {
        {
            0,  // flags;
            VK_PIPELINE_BIND_POINT_GRAPHICS,  // pipelineBindPoint;
            0,  // inputAttachmentCount;
            nullptr,  // pInputAttachments;
            1,  // colorAttachmentCount;
            color_attachment_references,  // pColorAttachments;
            nullptr,  // pResolveAttachments;
            depth_attachment_references,  // pDepthStencilAttachment;
            0,  // preserveAttachmentCount;
            nullptr  // pPreserveAttachments;
        }
    };

    std::vector<VkSubpassDependency> dependencies = {
        {
            VK_SUBPASS_EXTERNAL,  // srcSubpass;
            0,  // dstSubpass;
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask;
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // dstStageMask;
            0,  // srcAccessMask;
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,  // dstAccessMask;
            VK_DEPENDENCY_BY_REGION_BIT  // dependencyFlags;
        }
    };

    VkRenderPassCreateInfo create_info = {
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        (uint32_t)attachment_descriptions.size(),  // attachmentCount;
        attachment_descriptions.data(),  // pAttachments;
        1,  // subpassCount;
        subpass_descriptions,  // pSubpasses;
        (uint32_t)dependencies.size(),  // dependencyCount;
        dependencies.data()  // pDependencies;
    };
    VkRenderPass render_pass = VK_NULL_HANDLE;
    vkres = vkCreateRenderPass(device, &create_info, vulkan_allocator, &render_pass);
    assert(vkres == VK_SUCCESS);

    return render_pass;
}

VkRenderPass create_depth_render_pass(VkDevice device, VkFormat depth_format)
{
    // from SaschaWillems examples/shadowmapping
    VkResult vkres;
    std::vector<VkAttachmentDescription> attachment_descriptions{
        {
            0,  // flags;
            depth_format,  // format;
            VK_SAMPLE_COUNT_1_BIT,  // samples;
            VK_ATTACHMENT_LOAD_OP_CLEAR,  // loadOp;
            VK_ATTACHMENT_STORE_OP_STORE,  // storeOp;
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // stencilLoadOp;
            VK_ATTACHMENT_STORE_OP_DONT_CARE,  // stencilStoreOp;
            VK_IMAGE_LAYOUT_UNDEFINED,  // initialLayout;
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL  // finalLayout;
        }
    };
    VkAttachmentReference depth_attachment_references[1] = {
        {
            0,  // attachment
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // layout;
        }
    };
    VkSubpassDescription subpass_descriptions[1] = {
        {
            0,  // flags;
            VK_PIPELINE_BIND_POINT_GRAPHICS,  // pipelineBindPoint;
            0,  // inputAttachmentCount;
            nullptr,  // pInputAttachments;
            0,  // colorAttachmentCount;
            nullptr,  // pColorAttachments;
            nullptr,  // pResolveAttachments;
            depth_attachment_references,  // pDepthStencilAttachment;
            0,  // preserveAttachmentCount;
            nullptr  // pPreserveAttachments;
        }
    };

    std::vector<VkSubpassDependency> dependencies = {
        {
            VK_SUBPASS_EXTERNAL,  // srcSubpass;
            0,  // dstSubpass;
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,  // srcStageMask;
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,  // dstStageMask;
            VK_ACCESS_SHADER_READ_BIT,  // srcAccessMask;
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,  // dstAccessMask;
            VK_DEPENDENCY_BY_REGION_BIT  // dependencyFlags;
        },
        {
            0,  // srcSubpass;
            VK_SUBPASS_EXTERNAL,  // dstSubpass;
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,  // srcStageMask;
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,  // dstStageMask;
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,  // srcAccessMask;
            VK_ACCESS_SHADER_READ_BIT,  // dstAccessMask;
            VK_DEPENDENCY_BY_REGION_BIT  // dependencyFlags;
        }
    };

    VkRenderPassCreateInfo create_info = {
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        (uint32_t)attachment_descriptions.size(),  // attachmentCount;
        attachment_descriptions.data(),  // pAttachments;
        1,  // subpassCount;
        subpass_descriptions,  // pSubpasses;
        (uint32_t)dependencies.size(),  // dependencyCount;
        dependencies.data()  // pDependencies;
    };
    VkRenderPass render_pass = VK_NULL_HANDLE;
    vkres = vkCreateRenderPass(device, &create_info, vulkan_allocator, &render_pass);
    assert(vkres == VK_SUCCESS);

    return render_pass;
}

VkDescriptorSetLayout create_descriptor_set_layout(VkDevice device) {
    VkResult vkres;
    std::array<VkDescriptorSetLayoutBinding, 2> layout_bindings =
    { {
        {
            0,  // binding;
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // descriptorType;
            1,  // descriptorCount;
            VK_SHADER_STAGE_FRAGMENT_BIT,  // stageFlags;
            nullptr  // pImmutableSamplers;
        },
        {
            1,  // binding;
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // descriptorType;
            1,  // descriptorCount;
            VK_SHADER_STAGE_VERTEX_BIT,  // stageFlags;
            nullptr  // pImmutableSamplers;
        }
    } };
    VkDescriptorSetLayoutCreateInfo create_info = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        (uint32_t)layout_bindings.size(),  // bindingCount;
        layout_bindings.data()  // pBindings;
    };
    VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
    vkres = vkCreateDescriptorSetLayout(device, &create_info, vulkan_allocator, &descriptor_set_layout);
    assert(vkres == VK_SUCCESS);
    return descriptor_set_layout;
}

void destroy_descriptor_set_layout(VkDevice device, VkDescriptorSetLayout descriptor_set_layout)
{
    vkDestroyDescriptorSetLayout(device, descriptor_set_layout, vulkan_allocator);
}

VkDescriptorPool create_descriptor_pool(VkDevice device, bool enable_free) {
    VkResult vkres;
    VkDescriptorPoolCreateFlags flags = 0;
    if (enable_free) {
        flags |= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    }
    std::uint32_t descriptor_max_count = 1000;
    std::array<VkDescriptorPoolSize, 2> pool_sizes =
    { {
        {
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // descriptorCount;
            descriptor_max_count  // count;
        },
        {
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // descriptorCount;
            descriptor_max_count  // count;
        }
    } };
    VkDescriptorPoolCreateInfo create_info = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        flags,  // flags;
        descriptor_max_count * (uint32_t)pool_sizes.size(),  // maxSets;
        (uint32_t)pool_sizes.size(),  // poolSizeCount;
        pool_sizes.data()  // pPoolSizes;
    };
    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    vkres = vkCreateDescriptorPool(device, &create_info, vulkan_allocator, &descriptor_pool);
    assert(vkres == VK_SUCCESS);
    return descriptor_pool;
}

void vkutil::reset_descriptor_pool(VkDevice device, VkDescriptorPool descriptor_pool)
{
    VkResult vkres;
    vkres = vkResetDescriptorPool(device, descriptor_pool, 0);
    check_vk_result(vkres);
}

void vkutil::destroy_descriptor_pool(VkDevice device, VkDescriptorPool descriptor_pool)
{
    vkDestroyDescriptorPool(device, descriptor_pool, vulkan_allocator);
}

VkDescriptorSet create_descriptor_set(VkDevice device,
    VkDescriptorPool descriptor_pool,
    VkDescriptorSetLayout descriptor_set_layout
) {
    VkResult vkres;
    VkDescriptorSetAllocateInfo allocate_info = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,  // sType;
        nullptr,  // pNext;
        descriptor_pool,  // descriptorPool;
        1,  // descriptorSetCount;
        &descriptor_set_layout  // pSetLayouts;
    };
    VkDescriptorSet descriptor_set;
    vkres = vkAllocateDescriptorSets(device, &allocate_info, &descriptor_set);
    assert(vkres == VK_SUCCESS);
    return descriptor_set;
}

void free_descriptor_sets(VkDevice device, VkDescriptorPool descriptor_pool, const std::vector<VkDescriptorSet>& descriptor_sets)
{
    if (descriptor_sets.empty()) {
        return;
    }
    vkFreeDescriptorSets(device, descriptor_pool, (uint32_t)descriptor_sets.size(), descriptor_sets.data());
}

VkShaderModule create_shader_module(VkDevice device, std::string filename) {
    VkResult vkres;
    std::vector<uint8_t> buffer = fileutil::read_binary_file(filename);
    VkShaderModuleCreateInfo create_info = {
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        buffer.size(),  // codeSize;
        (uint32_t*)buffer.data()  // pCode;
    };
    VkShaderModule shader_module;
    vkres = vkCreateShaderModule(device, &create_info, vulkan_allocator, &shader_module);
    assert(vkres == VK_SUCCESS);
    return shader_module;
}

VkPipelineLayout create_pipeline_layout(VkDevice device, VkDescriptorSetLayout descriptor_set_layout) {
    VkResult vkres;
    VkPipelineLayoutCreateInfo create_info = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        1,  // setLayoutCount;
        &descriptor_set_layout,  // pSetLayouts;
        0,  // pushConstantRangeCount;
        nullptr  // pPushConstantRanges;
    };
    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    vkres = vkCreatePipelineLayout(device, &create_info, vulkan_allocator, &pipeline_layout);
    assert(vkres == VK_SUCCESS);
    return pipeline_layout;
}

void destroy_pipeline(VkDevice device, VkPipeline pipeline)
{
    vkDestroyPipeline(device, pipeline, vulkan_allocator);
}

uint32_t find_memory_type_index(const VkPhysicalDeviceMemoryProperties& memory_properties, uint32_t memory_type_bits_required, VkMemoryPropertyFlags required_properties) {
    for (uint32_t memory_type_index = 0; memory_type_index < memory_properties.memoryTypeCount; ++memory_type_index) {
        uint32_t memory_type_mask = (1 << memory_type_index);
        bool is_required_memory_type = memory_type_bits_required & memory_type_mask;
        if (is_required_memory_type) {
            const VkMemoryType& memory_type = memory_properties.memoryTypes[memory_type_index];
            bool has_required_properties = (memory_type.propertyFlags & required_properties) == required_properties;
            if (has_required_properties) {
                return memory_type_index;
            }
        }
    }
    assert(false);
    return 0xffffffff;
}

VkDeviceMemory create_buffer_memory(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkBuffer buffer, VkMemoryPropertyFlags required_properties) {
    VkResult vkres;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

    uint32_t memory_type_index = find_memory_type_index(memory_properties, memory_requirements.memoryTypeBits, required_properties);

    VkMemoryAllocateInfo memory_allocate_info = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,  // sType;
        nullptr,  // pNext;
        memory_requirements.size,  // allocationSize;
        memory_type_index  // memoryTypeIndex;
    };
    vkres = vkAllocateMemory(device, &memory_allocate_info, vulkan_allocator, &memory);
    assert(vkres == VK_SUCCESS);
    return memory;
}

std::pair<VkBuffer, VkDeviceMemory> create_buffer(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkDeviceSize buffer_size, VkBufferUsageFlags usage, VkMemoryPropertyFlags required_properties) {
    VkResult vkres;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkBufferCreateInfo create_info = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        buffer_size,  // size;
        usage,  // usage;
        VK_SHARING_MODE_EXCLUSIVE,  // sharingMode;
        0,  // queueFamilyIndexCount;
        nullptr  // pQueueFamilyIndices;
    };
    vkres = vkCreateBuffer(device, &create_info, vulkan_allocator, &buffer);
    assert(vkres == VK_SUCCESS);

    VkDeviceMemory memory = create_buffer_memory(memory_properties, device, buffer, required_properties);
    vkBindBufferMemory(device, buffer, memory, 0);
    return std::make_pair(buffer, memory);
}

std::pair<VkBuffer, VkDeviceMemory> create_staging_buffer(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkDeviceSize buffer_size) {
    return create_buffer(memory_properties, device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
}

std::pair<VkBuffer, VkDeviceMemory> create_vertex_buffer(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, uint8_t* buffer_data, std::size_t buffer_size)
{
    VkResult vkres;

    auto staging_buffer_and_memory = create_staging_buffer(memory_properties, device, buffer_size);
    //auto staging_buffer_and_memory = create_staging_buffer(memory_properties, device, 0x80);
    /*
    * Size in pMemRanges[0] is VK_WHOLE_SIZE and allocationSize minus offset (0x60 - 0x0) is not a multiple of VkPhysicalDeviceLimits::nonCoherentAtomSize (0x80). The Vulkan spec states: If size is equal to VK_WHOLE_SIZE, the end of the current mapping of memory must be a multiple of VkPhysicalDeviceLimits::nonCoherentAtomSize bytes from the beginning of the memory object
    */
    // may be use host_coherent for staging buffer, then no need to flush
    VkBuffer staging_buffer = staging_buffer_and_memory.first;
    VkDeviceMemory staging_memory = staging_buffer_and_memory.second;

    void* memory_pointer;
    vkres = vkMapMemory(device, staging_memory, 0, buffer_size, 0, &memory_pointer);
    assert(vkres == VK_SUCCESS);

    memcpy(memory_pointer, buffer_data, buffer_size);

    VkMappedMemoryRange flush_range = {
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // sType;
        nullptr,  // pNext;
        staging_memory,  // memory;
        0, // offset;
        VK_WHOLE_SIZE  // size;
    };
    vkFlushMappedMemoryRanges(device, 1, &flush_range);
    vkUnmapMemory(device, staging_memory);

    // 
    auto buffer_and_memory = create_buffer(memory_properties, device, buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkBuffer buffer = buffer_and_memory.first;
    VkDeviceMemory memory = buffer_and_memory.second;

    VkCommandBufferBeginInfo begin_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // sType;
        nullptr,  // pNext;
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,  // flags;
        nullptr  // pInheritanceInfo;
    };
    vkBeginCommandBuffer(command_buffer, &begin_info);
    VkBufferCopy region = {
        0,  // srcOffset;
        0,  // dstOffset;
        buffer_size  // size;
    };
    vkCmdCopyBuffer(command_buffer, staging_buffer, buffer, 1, &region);

    VkBufferMemoryBarrier buffer_memory_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,  // sType;
        nullptr,  // pNext;
        VK_ACCESS_MEMORY_WRITE_BIT,  // srcAccessMask;
        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,  // dstAccessMask;
        VK_QUEUE_FAMILY_IGNORED,  // srcQueueFamilyIndex;
        VK_QUEUE_FAMILY_IGNORED,  // dstQueueFamilyIndex;
        buffer,  // buffer;
        0,  // offset;
        VK_WHOLE_SIZE  // size;
    };
    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr,
        1, &buffer_memory_barrier, 0, nullptr);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType;
        nullptr,  // pNext;
        0,  // waitSemaphoreCount;
        nullptr,  // pWaitSemaphores;
        0,  // pWaitDstStageMask;
        1,  // commandBufferCount;
        &command_buffer,  // pCommandBuffers;
        0,  // signalSemaphoreCount;
        nullptr  // pSignalSemaphores;
    };

    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkDestroyBuffer(device, staging_buffer, vulkan_allocator);
    vkFreeMemory(device, staging_memory, vulkan_allocator);

    return std::make_pair(buffer, memory);
}

std::pair<VkBuffer, VkDeviceMemory> create_index_buffer(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, uint8_t* buffer_data, std::size_t buffer_size)
{
    VkResult vkres;

    auto staging_buffer_and_memory = create_staging_buffer(memory_properties, device, buffer_size);
    //auto staging_buffer_and_memory = create_staging_buffer(memory_properties, device, 0x80);
    /*
    * Size in pMemRanges[0] is VK_WHOLE_SIZE and allocationSize minus offset (0x60 - 0x0) is not a multiple of VkPhysicalDeviceLimits::nonCoherentAtomSize (0x80). The Vulkan spec states: If size is equal to VK_WHOLE_SIZE, the end of the current mapping of memory must be a multiple of VkPhysicalDeviceLimits::nonCoherentAtomSize bytes from the beginning of the memory object
    */
    // may be use host_coherent for staging buffer, then no need to flush
    VkBuffer staging_buffer = staging_buffer_and_memory.first;
    VkDeviceMemory staging_memory = staging_buffer_and_memory.second;

    void* memory_pointer;
    vkres = vkMapMemory(device, staging_memory, 0, buffer_size, 0, &memory_pointer);
    assert(vkres == VK_SUCCESS);

    memcpy(memory_pointer, buffer_data, buffer_size);

    VkMappedMemoryRange flush_range = {
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // sType;
        nullptr,  // pNext;
        staging_memory,  // memory;
        0, // offset;
        VK_WHOLE_SIZE  // size;
    };
    vkFlushMappedMemoryRanges(device, 1, &flush_range);
    vkUnmapMemory(device, staging_memory);

    // 
    auto buffer_and_memory = create_buffer(memory_properties, device, buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkBuffer buffer = buffer_and_memory.first;
    VkDeviceMemory memory = buffer_and_memory.second;

    VkCommandBufferBeginInfo begin_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // sType;
        nullptr,  // pNext;
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,  // flags;
        nullptr  // pInheritanceInfo;
    };
    vkBeginCommandBuffer(command_buffer, &begin_info);
    VkBufferCopy region = {
        0,  // srcOffset;
        0,  // dstOffset;
        buffer_size  // size;
    };
    vkCmdCopyBuffer(command_buffer, staging_buffer, buffer, 1, &region);

    VkBufferMemoryBarrier buffer_memory_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,  // sType;
        nullptr,  // pNext;
        VK_ACCESS_MEMORY_WRITE_BIT,  // srcAccessMask;
        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,  // dstAccessMask;
        VK_QUEUE_FAMILY_IGNORED,  // srcQueueFamilyIndex;
        VK_QUEUE_FAMILY_IGNORED,  // dstQueueFamilyIndex;
        buffer,  // buffer;
        0,  // offset;
        VK_WHOLE_SIZE  // size;
    };
    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr,
        1, &buffer_memory_barrier, 0, nullptr);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType;
        nullptr,  // pNext;
        0,  // waitSemaphoreCount;
        nullptr,  // pWaitSemaphores;
        0,  // pWaitDstStageMask;
        1,  // commandBufferCount;
        &command_buffer,  // pCommandBuffers;
        0,  // signalSemaphoreCount;
        nullptr  // pSignalSemaphores;
    };

    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkDestroyBuffer(device, staging_buffer, vulkan_allocator);
    vkFreeMemory(device, staging_memory, vulkan_allocator);

    return std::make_pair(buffer, memory);
}

VkImage create_image(VkDevice device, uint32_t width, uint32_t height) {
    VkExtent3D extent = {
        width, height, 1
    };
    VkImageCreateInfo create_info = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        VK_IMAGE_TYPE_2D,  // imageType;
        VK_FORMAT_R8G8B8A8_UNORM,  // format;  // TODO different format
        extent,  // extent;
        1,  // mipLevels;
        1,  // arrayLayers;
        VK_SAMPLE_COUNT_1_BIT,  // samples;
        VK_IMAGE_TILING_OPTIMAL,  // tiling;
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,  // usage;
        VK_SHARING_MODE_EXCLUSIVE,  // sharingMode;
        0,  // queueFamilyIndexCount;
        nullptr,  // pQueueFamilyIndices;
        VK_IMAGE_LAYOUT_UNDEFINED  // initialLayout;
    };
    VkImage image = VK_NULL_HANDLE;
    vkCreateImage(device, &create_info, vulkan_allocator, &image);
    return image;
}

VkImage create_image_depth(VkDevice device, VkFormat format, uint32_t width, uint32_t height, uint32_t array_layers, bool sampled) {
    VkExtent3D extent = {
        width, height, 1
    };
    VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if (sampled) {
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    VkImageCreateInfo create_info = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        VK_IMAGE_TYPE_2D,  // imageType;
        format,  // format;
        extent,  // extent;
        1,  // mipLevels;
        array_layers,  // arrayLayers;
        VK_SAMPLE_COUNT_1_BIT,  // samples;
        VK_IMAGE_TILING_OPTIMAL,  // tiling;
        usage,  // usage;
        VK_SHARING_MODE_EXCLUSIVE,  // sharingMode;
        0,  // queueFamilyIndexCount;
        nullptr,  // pQueueFamilyIndices;
        VK_IMAGE_LAYOUT_UNDEFINED  // initialLayout;
    };
    VkImage image = VK_NULL_HANDLE;
    vkCreateImage(device, &create_info, vulkan_allocator, &image);
    return image;
}

VkDeviceMemory create_image_memory(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkImage image, VkMemoryPropertyFlags required_properties) {
    VkResult vkres;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(device, image, &memory_requirements);

    uint32_t memory_type_index = find_memory_type_index(memory_properties, memory_requirements.memoryTypeBits, required_properties);

    VkMemoryAllocateInfo memory_allocate_info = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,  // sType;
        nullptr,  // pNext;
        memory_requirements.size,  // allocationSize;
        memory_type_index  // memoryTypeIndex;
    };
    vkres = vkAllocateMemory(device, &memory_allocate_info, vulkan_allocator, &memory);
    assert(vkres == VK_SUCCESS);
    return memory;
}

std::pair<VkImage, VkDeviceMemory> create_texture(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, uint32_t width, uint32_t height, uint8_t* buffer_data, std::size_t buffer_size)
{
    VkResult vkres;
    VkImage image = create_image(device, width, height);
    VkDeviceMemory memory = create_image_memory(memory_properties, device, image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkres = vkBindImageMemory(device, image, memory, 0);
    assert(vkres == VK_SUCCESS);

    auto staging_buffer_and_memory = create_staging_buffer(memory_properties, device, buffer_size);
    VkBuffer staging_buffer = staging_buffer_and_memory.first;
    VkDeviceMemory staging_memory = staging_buffer_and_memory.second;

    void* memory_pointer;
    vkres = vkMapMemory(device, staging_memory, 0, buffer_size, 0, &memory_pointer);
    assert(vkres == VK_SUCCESS);

    memcpy(memory_pointer, buffer_data, buffer_size);

    VkMappedMemoryRange flush_range = {
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // sType;
        nullptr,  // pNext;
        staging_memory,  // memory;
        0, // offset;
        VK_WHOLE_SIZE  // size;
    };
    vkFlushMappedMemoryRanges(device, 1, &flush_range);
    vkUnmapMemory(device, staging_memory);

    VkCommandBufferBeginInfo begin_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // sType;
        nullptr,  // pNext;
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,  // flags;
        nullptr  // pInheritanceInfo;
    };
    vkBeginCommandBuffer(command_buffer, &begin_info);

    VkImageSubresourceRange image_subresource_range = {
        VK_IMAGE_ASPECT_COLOR_BIT,  // aspectMask;
        0,  // baseMipLevel;
        1,  // levelCount;
        0,  // baseArrayLayer;
        1  // layerCount;
    };
    VkImageMemoryBarrier image_memory_barrier_from_undefined_to_transfer_dst = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,  // sType;
        nullptr,  // pNext;
        0,  // srcAccessMask;
        VK_ACCESS_TRANSFER_WRITE_BIT,  // dstAccessMask;
        VK_IMAGE_LAYOUT_UNDEFINED,  // oldLayout;
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,  // newLayout;
        VK_QUEUE_FAMILY_IGNORED,  // srcQueueFamilyIndex;
        VK_QUEUE_FAMILY_IGNORED,  // dstQueueFamilyIndex;
        image,  // image;
        image_subresource_range  // subresourceRange;
    };
    vkCmdPipelineBarrier(command_buffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
        0, nullptr, 0, nullptr,
        1, &image_memory_barrier_from_undefined_to_transfer_dst);

    VkBufferImageCopy region = {
        0,  // bufferOffset;
        0,  // bufferRowLength;
        0,  // bufferImageHeight;
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            0,
            1
        },  // imageSubresource;
        {
            0,
            0,
            0
        },// imageOffset;
        {
            width,
            height,
            1,
        }// imageExtent;
    };
    vkCmdCopyBufferToImage(command_buffer, staging_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VkImageMemoryBarrier image_memory_barrier_from_transfer_dst_to_shader_read = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,  // sType;
        nullptr,  // pNext;
        VK_ACCESS_TRANSFER_WRITE_BIT,  // srcAccessMask;
        VK_ACCESS_SHADER_READ_BIT,  // dstAccessMask;
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,  // oldLayout;
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,  // newLayout;
        VK_QUEUE_FAMILY_IGNORED,  // srcQueueFamilyIndex;
        VK_QUEUE_FAMILY_IGNORED,  // dstQueueFamilyIndex;
        image,  // image;
        image_subresource_range  // subresourceRange;
    };

    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr, 0, nullptr,
        1, &image_memory_barrier_from_transfer_dst_to_shader_read);


    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType;
        nullptr,  // pNext;
        0,  // waitSemaphoreCount;
        nullptr,  // pWaitSemaphores;
        0,  // pWaitDstStageMask;
        1,  // commandBufferCount;
        &command_buffer,  // pCommandBuffers;
        0,  // signalSemaphoreCount;
        nullptr  // pSignalSemaphores;
    };

    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkDestroyBuffer(device, staging_buffer, vulkan_allocator);
    vkFreeMemory(device, staging_memory, vulkan_allocator);

    return std::make_pair(image, memory);
}

VkImageView create_image_view_texture(VkDevice device, VkImage image) {
    VkImageViewCreateInfo create_info = {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        image,  // image;
        VK_IMAGE_VIEW_TYPE_2D,  // viewType;
        VK_FORMAT_R8G8B8A8_UNORM,  // format;
        {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        },  // components;
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            1
        }  // subresourceRange;
    };
    VkImageView image_view = VK_NULL_HANDLE;
    vkCreateImageView(device, &create_info, vulkan_allocator, &image_view);
    return image_view;
}

VkSampler create_sampler(VkDevice device) {
    VkSamplerCreateInfo create_info = {
        VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        VK_FILTER_LINEAR,  // magFilter;
        VK_FILTER_LINEAR,  // minFilter;
        VK_SAMPLER_MIPMAP_MODE_NEAREST,  // mipmapMode;
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // addressModeU;
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // addressModeV;
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // addressModeW;
        0.0f,  // mipLodBias;
        VK_FALSE,  // anisotropyEnable;
        1.0f,  // maxAnisotropy;
        VK_FALSE,  // compareEnable;
        VK_COMPARE_OP_ALWAYS,  // compareOp;
        0.0f,  // minLod;
        0.0f,  // maxLod;
        VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,  // borderColor;
        VK_FALSE  // unnormalizedCoordinates;
    };
    VkSampler sampler = VK_NULL_HANDLE;
    vkCreateSampler(device, &create_info, vulkan_allocator, &sampler);
    return sampler;
}

VkSampler create_depth_sampler(VkDevice device) {
    VkResult vkres;
    VkSamplerCreateInfo create_info = {
        VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        VK_FILTER_LINEAR,  // magFilter;
        VK_FILTER_LINEAR,  // minFilter;
        VK_SAMPLER_MIPMAP_MODE_LINEAR,  // mipmapMode;
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // addressModeU;
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // addressModeV;
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // addressModeW;
        0.0f,  // mipLodBias;
        VK_FALSE,  // anisotropyEnable;
        1.0f,  // maxAnisotropy;
        VK_FALSE,  // compareEnable;
        VK_COMPARE_OP_ALWAYS,  // compareOp;
        0.0f,  // minLod;
        1.0f,  // maxLod;
        VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,  // borderColor;
        VK_FALSE  // unnormalizedCoordinates;
    };
    VkSampler sampler = VK_NULL_HANDLE;
    vkres = vkCreateSampler(device, &create_info, vulkan_allocator, &sampler);
    check_vk_result(vkres);
    return sampler;
}

VkSampler create_shadow_sampler(VkDevice device) {
    VkResult vkres;
    VkSamplerCreateInfo create_info = {
        VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        VK_FILTER_LINEAR,  // magFilter;
        VK_FILTER_LINEAR,  // minFilter;
        VK_SAMPLER_MIPMAP_MODE_LINEAR,  // mipmapMode;
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // addressModeU;
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // addressModeV;
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,  // addressModeW;
        0.0f,  // mipLodBias;
        VK_FALSE,  // anisotropyEnable;
        1.0f,  // maxAnisotropy;
        VK_TRUE,  // compareEnable;
        VK_COMPARE_OP_LESS,  // compareOp;
        0.0f,  // minLod;
        1.0f,  // maxLod;
        VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,  // borderColor;
        VK_FALSE  // unnormalizedCoordinates;
    };
    VkSampler sampler = VK_NULL_HANDLE;
    vkres = vkCreateSampler(device, &create_info, vulkan_allocator, &sampler);
    check_vk_result(vkres);
    return sampler;
}

VkImage create_image_cubemap(VkDevice device, uint32_t width, uint32_t height)
{
    VkExtent3D extent = {
        width, height, 1
    };
    VkImageCreateInfo create_info = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,  // flags;
        VK_IMAGE_TYPE_2D,  // imageType;
        VK_FORMAT_R8G8B8A8_UNORM,  // format;
        extent,  // extent;
        1,  // mipLevels;
        6,  // arrayLayers;
        VK_SAMPLE_COUNT_1_BIT,  // samples;
        VK_IMAGE_TILING_OPTIMAL,  // tiling;
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,  // usage;
        VK_SHARING_MODE_EXCLUSIVE,  // sharingMode;
        0,  // queueFamilyIndexCount;
        nullptr,  // pQueueFamilyIndices;
        VK_IMAGE_LAYOUT_UNDEFINED  // initialLayout;
    };
    VkImage image = VK_NULL_HANDLE;
    vkCreateImage(device, &create_info, vulkan_allocator, &image);
    return image;
}

std::pair<VkImage, VkDeviceMemory> create_texture_cubemap(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, uint32_t width, uint32_t height, uint8_t* buffer_data, std::size_t buffer_size)
{
    VkResult vkres;
    VkImage image = create_image_cubemap(device, width, height);
    VkDeviceMemory memory = create_image_memory(memory_properties, device, image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkres = vkBindImageMemory(device, image, memory, 0);
    assert(vkres == VK_SUCCESS);

    auto staging_buffer_and_memory = create_staging_buffer_coherent(memory_properties, device, buffer_size);
    VkBuffer staging_buffer = staging_buffer_and_memory.first;
    VkDeviceMemory staging_memory = staging_buffer_and_memory.second;

    void* memory_pointer;
    vkres = vkMapMemory(device, staging_memory, 0, buffer_size, 0, &memory_pointer);
    assert(vkres == VK_SUCCESS);
    memcpy(memory_pointer, buffer_data, buffer_size);
    vkUnmapMemory(device, staging_memory);

    VkCommandBufferBeginInfo begin_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // sType;
        nullptr,  // pNext;
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,  // flags;
        nullptr  // pInheritanceInfo;
    };
    vkBeginCommandBuffer(command_buffer, &begin_info);

    VkImageSubresourceRange image_subresource_range = {
        VK_IMAGE_ASPECT_COLOR_BIT,  // aspectMask;
        0,  // baseMipLevel;
        1,  // levelCount;
        0,  // baseArrayLayer;
        6  // layerCount;
    };
    VkImageMemoryBarrier image_memory_barrier_from_undefined_to_transfer_dst = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,  // sType;
        nullptr,  // pNext;
        0,  // srcAccessMask;
        VK_ACCESS_TRANSFER_WRITE_BIT,  // dstAccessMask;
        VK_IMAGE_LAYOUT_UNDEFINED,  // oldLayout;
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,  // newLayout;
        VK_QUEUE_FAMILY_IGNORED,  // srcQueueFamilyIndex;
        VK_QUEUE_FAMILY_IGNORED,  // dstQueueFamilyIndex;
        image,  // image;
        image_subresource_range  // subresourceRange;
    };
    vkCmdPipelineBarrier(command_buffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
        0, nullptr, 0, nullptr,
        1, &image_memory_barrier_from_undefined_to_transfer_dst);

    VkBufferImageCopy region = {
        0,  // bufferOffset;
        0,  // bufferRowLength;
        0,  // bufferImageHeight;
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            0,
            6
        },  // imageSubresource;
        {
            0,
            0,
            0
        },// imageOffset;
        {
            width,
            height,
            1,
        }// imageExtent;
    };
    vkCmdCopyBufferToImage(command_buffer, staging_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VkImageMemoryBarrier image_memory_barrier_from_transfer_dst_to_shader_read = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,  // sType;
        nullptr,  // pNext;
        VK_ACCESS_TRANSFER_WRITE_BIT,  // srcAccessMask;
        VK_ACCESS_SHADER_READ_BIT,  // dstAccessMask;
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,  // oldLayout;
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,  // newLayout;
        VK_QUEUE_FAMILY_IGNORED,  // srcQueueFamilyIndex;
        VK_QUEUE_FAMILY_IGNORED,  // dstQueueFamilyIndex;
        image,  // image;
        image_subresource_range  // subresourceRange;
    };

    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr, 0, nullptr,
        1, &image_memory_barrier_from_transfer_dst_to_shader_read);


    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType;
        nullptr,  // pNext;
        0,  // waitSemaphoreCount;
        nullptr,  // pWaitSemaphores;
        0,  // pWaitDstStageMask;
        1,  // commandBufferCount;
        &command_buffer,  // pCommandBuffers;
        0,  // signalSemaphoreCount;
        nullptr  // pSignalSemaphores;
    };

    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkDestroyBuffer(device, staging_buffer, vulkan_allocator);
    vkFreeMemory(device, staging_memory, vulkan_allocator);

    return std::make_pair(image, memory);
}

VkImageView create_image_view_texture_cubemap(VkDevice device, VkImage image)
{
    VkImageViewCreateInfo create_info = {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        image,  // image;
        VK_IMAGE_VIEW_TYPE_CUBE,  // viewType;
        VK_FORMAT_R8G8B8A8_UNORM,  // format;
        {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        },  // components;
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            6
        }  // subresourceRange;
    };
    VkImageView image_view = VK_NULL_HANDLE;
    vkCreateImageView(device, &create_info, vulkan_allocator, &image_view);
    return image_view;
}

std::pair<VkImage, VkDeviceMemory> create_texture_depth(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, VkFormat format, uint32_t width, uint32_t height, uint32_t array_layers, bool sampled)
{
    (void)command_buffer;
    (void)queue;

    VkResult vkres;
    VkImage image = create_image_depth(device, format, width, height, array_layers, sampled);
    VkDeviceMemory memory = create_image_memory(memory_properties, device, image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkres = vkBindImageMemory(device, image, memory, 0);
    assert(vkres == VK_SUCCESS);
    return std::make_pair(image, memory);
}

VkImageView create_image_view_texture_depth(VkDevice device, VkImage image, VkFormat format, bool use_depth, bool use_stencil, uint32_t base_array_layer, uint32_t layer_count)
{
    VkImageAspectFlags image_aspect_flags = 0;
    if (use_depth) {
        image_aspect_flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    if (use_stencil) {
        image_aspect_flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    VkImageViewType view_type;
    if (layer_count == 1) {
        view_type = VK_IMAGE_VIEW_TYPE_2D;
    }
    else {
        view_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    }
    VkImageViewCreateInfo create_info = {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        image,  // image;
        view_type,  // viewType;
        format,  // format;
        {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        },  // components;
        {
            image_aspect_flags,
            0,
            1,
            base_array_layer,
            layer_count
        }  // subresourceRange;
    };
    VkImageView image_view = VK_NULL_HANDLE;
    vkCreateImageView(device, &create_info, vulkan_allocator, &image_view);
    return image_view;
}

std::pair<VkBuffer, VkDeviceMemory> create_staging_buffer_coherent(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkDeviceSize buffer_size) {
    return create_buffer(memory_properties, device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

std::pair<VkBuffer, VkDeviceMemory> create_uniform_buffer(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device,
    VkQueue queue, VkCommandBuffer command_buffer,
    uint8_t* buffer_data, VkDeviceSize buffer_size) {

    VkResult vkres;
    auto staging_buffer_and_memory = create_staging_buffer_coherent(memory_properties, device, buffer_size);
    VkBuffer staging_buffer = staging_buffer_and_memory.first;
    VkDeviceMemory staging_memory = staging_buffer_and_memory.second;

    void* memory_pointer;
    vkres = vkMapMemory(device, staging_memory, 0, buffer_size, 0, &memory_pointer);
    assert(vkres == VK_SUCCESS);
    memcpy(memory_pointer, buffer_data, buffer_size);
    vkUnmapMemory(device, staging_memory);

    auto buffer_and_memory = create_buffer(memory_properties, device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VkBuffer buffer = buffer_and_memory.first;
    VkDeviceMemory memory = buffer_and_memory.second;


    VkCommandBufferBeginInfo begin_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // sType;
        nullptr,  // pNext;
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,  // flags;
        nullptr  // pInheritanceInfo;
    };
    vkBeginCommandBuffer(command_buffer, &begin_info);
    VkBufferCopy region = {
        0,  // srcOffset;
        0,  // dstOffset;
        buffer_size  // size;
    };
    vkCmdCopyBuffer(command_buffer, staging_buffer, buffer, 1, &region);

    VkBufferMemoryBarrier buffer_memory_barrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,  // sType;
        nullptr,  // pNext;
        VK_ACCESS_TRANSFER_WRITE_BIT,  // srcAccessMask;
        VK_ACCESS_UNIFORM_READ_BIT,  // dstAccessMask;
        VK_QUEUE_FAMILY_IGNORED,  // srcQueueFamilyIndex;
        VK_QUEUE_FAMILY_IGNORED,  // dstQueueFamilyIndex;
        buffer,  // buffer;
        0,  // offset;
        VK_WHOLE_SIZE  // size;
    };
    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 0, 0, nullptr,
        1, &buffer_memory_barrier, 0, nullptr);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType;
        nullptr,  // pNext;
        0,  // waitSemaphoreCount;
        nullptr,  // pWaitSemaphores;
        0,  // pWaitDstStageMask;
        1,  // commandBufferCount;
        &command_buffer,  // pCommandBuffers;
        0,  // signalSemaphoreCount;
        nullptr  // pSignalSemaphores;
    };

    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkDestroyBuffer(device, staging_buffer, vulkan_allocator);
    vkFreeMemory(device, staging_memory, vulkan_allocator);

    return std::make_pair(buffer, memory);
}

std::pair<VkBuffer, VkDeviceMemory> create_uniform_buffer_coherent(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, uint8_t* buffer_data, VkDeviceSize buffer_size)
{
    (void)command_buffer;
    (void)queue;

    VkResult vkres;
    auto buffer_and_memory = create_buffer(memory_properties, device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkBuffer buffer = buffer_and_memory.first;
    VkDeviceMemory memory = buffer_and_memory.second;

    if (buffer_data != nullptr) {
        void* memory_pointer;
        vkres = vkMapMemory(device, memory, 0, buffer_size, 0, &memory_pointer);
        assert(vkres == VK_SUCCESS);
        memcpy(memory_pointer, buffer_data, buffer_size);
        vkUnmapMemory(device, memory);
    }

    return std::make_pair(buffer, memory);
}

void update_descriptor_set(VkDevice device, VkDescriptorSet descriptor_set, VkSampler sampler, VkImageView image_view, VkBuffer uniform_buffer, VkDeviceSize uniform_buffer_size) {
    VkDescriptorImageInfo image_info = {
        sampler,  // sampler;
        image_view,  // imageView;
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL  // imageLayout;
    };
    VkDescriptorBufferInfo uniform_buffer_info = {
        uniform_buffer,  // buffer;
        0,  // offset;
        uniform_buffer_size  // range;
    };
    std::array<VkWriteDescriptorSet, 2> descriptor_writes =
    { {
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,  // sType;
            nullptr,  // pNext;
            descriptor_set,  // dstSet;
            0,  // dstBinding;
            0,  // dstArrayElement;
            1,  // descriptorCount;
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,  // descriptorType;
            &image_info,  // pImageInfo;
            nullptr,  // pBufferInfo;
            nullptr  // pTexelBufferView;
        },
        {
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,  // sType;
            nullptr,  // pNext;
            descriptor_set,  // dstSet;
            1,  // dstBinding;
            0,  // dstArrayElement;
            1,  // descriptorCount;
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,  // descriptorType;
            nullptr,  // pImageInfo;
            &uniform_buffer_info,  // pBufferInfo;
            nullptr  // pTexelBufferView;
        }
     } };
    vkUpdateDescriptorSets(device, (uint32_t)descriptor_writes.size(), descriptor_writes.data(), 0, nullptr);
}

VkDescriptorSetLayout create_descriptor_set_layout(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& layout_bindings)
{
    VkResult vkres;
    VkDescriptorSetLayoutCreateInfo create_info = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        (uint32_t)layout_bindings.size(),  // bindingCount;
        layout_bindings.data()  // pBindings;
    };
    VkDescriptorSetLayout descriptor_set_layout;
    vkres = vkCreateDescriptorSetLayout(device, &create_info, vulkan_allocator, &descriptor_set_layout);
    check_vk_result(vkres);
    return descriptor_set_layout;
}

VkPipelineLayout create_pipeline_layout(VkDevice device, const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
    std::vector<VkPushConstantRange>& push_constant_ranges
    )
{
    VkResult vkres;
    VkPipelineLayoutCreateInfo create_info = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        (uint32_t)descriptor_set_layouts.size(),  // setLayoutCount;
        descriptor_set_layouts.data(),  // pSetLayouts;
        (uint32_t)push_constant_ranges.size(),  // pushConstantRangeCount;
        push_constant_ranges.data()  // pPushConstantRanges;
    };
    VkPipelineLayout pipeline_layout;
    vkres = vkCreatePipelineLayout(device, &create_info, vulkan_allocator, &pipeline_layout);
    check_vk_result(vkres);
    return pipeline_layout;
}

void destroy_pipeline_layout(VkDevice device, VkPipelineLayout pipeline_layout)
{
    vkDestroyPipelineLayout(device, pipeline_layout, vulkan_allocator);
}

VkPipeline create_pipeline(VkDevice device, VkRenderPass render_pass, uint32_t width, uint32_t height, std::string filename_vert_spv, std::string filename_frag_spv, 
    VkPipelineVertexInputStateCreateInfo vertex_input_state,
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state,
    VkPipelineLayout pipeline_layout, 
    VkCullModeFlags cull_model_flags)
{
    VkResult vkres;
    VkShaderModule vertex_shader_module = create_shader_module(device, filename_vert_spv);
    VkShaderModule fragment_shader_module = create_shader_module(device, filename_frag_spv);


    std::vector<VkPipelineShaderStageCreateInfo> stage_create_infos = {
        { // Vertex
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,  // sType;
            nullptr,  // pNext;
            0,  // flags;
            VK_SHADER_STAGE_VERTEX_BIT,  // stage;
            vertex_shader_module, // module;
            "main",  // pName;
            nullptr  // pSpecializationInfo;
        },
        { // Fragment
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,  // sType;
            nullptr,  // pNext;
            0,  // flags;
            VK_SHADER_STAGE_FRAGMENT_BIT,  // stage;
            fragment_shader_module,  // module;
            "main",  // pName;
            nullptr  // pSpecializationInfo;
        }
    };

    VkViewport viewport = {
        0.0f,  // x;
        0.0f,  // y;
        (float)width,  // width;
        (float)height,  // height;
        0.0f,  // minDepth;
        1.0f  // maxDepth;
    };
    VkRect2D scissor = {
        { // offset
            0,
            0
        },
        { // extent
            width,
            height
        }
    };
    VkPipelineViewportStateCreateInfo viewport_state = {
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        1,  // viewportCount;
        nullptr,  // pViewports;
        1,  // scissorCount;
        nullptr  // pScissors;
    };

    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        (uint32_t)dynamic_states.size(),  // dynamicStateCount;
        dynamic_states.data()  // pDynamicStates;
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        VK_FALSE,  // depthClampEnable;
        VK_FALSE,  // rasterizerDiscardEnable;
        VK_POLYGON_MODE_FILL,  // polygonMode;
        cull_model_flags,  // cullMode;
        VK_FRONT_FACE_COUNTER_CLOCKWISE,  // frontFace;
        VK_FALSE,  // depthBiasEnable;
        0.0f,  // depthBiasConstantFactor;
        0.0f,  // depthBiasClamp;
        0.0f,  // depthBiasSlopeFactor;
        1.0f  // lineWidth;
    };

    VkPipelineMultisampleStateCreateInfo multisample_state = {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        VK_SAMPLE_COUNT_1_BIT,  // rasterizationSamples;
        VK_FALSE,  // sampleShadingEnable;
        1.0f,  // minSampleShading;
        nullptr,  // pSampleMask;
        VK_FALSE,  // alphaToCoverageEnable;
        VK_FALSE  // alphaToOneEnable;
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        VK_TRUE,  // depthTestEnable;
        VK_TRUE,  // depthWriteEnable;
        VK_COMPARE_OP_LESS_OR_EQUAL,  // depthCompareOp;
        VK_FALSE,  // depthBoundsTestEnable;
        VK_FALSE,  // stencilTestEnable;
        {
            // failOp;
            // passOp;
            // depthFailOp;
            // compareOp;
            // compareMask;
            // writeMask;
            // reference;
        },  // front;
        {
            // failOp;
            // passOp;
            // depthFailOp;
            // compareOp;
            // compareMask;
            // writeMask;
            // reference;
        },  // back;
        0.0f,  // minDepthBounds;
        0.0f  // maxDepthBounds;
    };

    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
        VK_TRUE,  // blendEnable;
        VK_BLEND_FACTOR_SRC_ALPHA,  // srcColorBlendFactor;
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,  // dstColorBlendFactor;
        VK_BLEND_OP_ADD,  // colorBlendOp;
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,  // srcAlphaBlendFactor;
        VK_BLEND_FACTOR_ZERO,  // dstAlphaBlendFactor;
        VK_BLEND_OP_ADD,  // alphaBlendOp;
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT  // colorWriteMask;
    };
    VkPipelineColorBlendStateCreateInfo color_blend_state = {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        VK_FALSE,  // logicOpEnable;
        VK_LOGIC_OP_COPY,  // logicOp;
        1,  // attachmentCount;
        &color_blend_attachment_state,  // pAttachments;
        {0.0f, 0.0f, 0.0f, 0.0f}  // blendConstants[4];
    };

    VkGraphicsPipelineCreateInfo create_info = {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,  // sType;
        nullptr,  // pNext;
        0,  // flags;
        (uint32_t)stage_create_infos.size(),  // stageCount;
        stage_create_infos.data(),  // pStages;
        &vertex_input_state,  // pVertexInputState;
        &input_assembly_state,  // pInputAssemblyState;
        nullptr,  // pTessellationState;
        &viewport_state,  // pViewportState;
        &rasterization_state,  // pRasterizationState;
        &multisample_state,  // pMultisampleState;
        &depth_stencil_state,  // pDepthStencilState;
        &color_blend_state,  // pColorBlendState;
        &dynamic_state_create_info,  // pDynamicState;
        pipeline_layout,  // layout;
        render_pass,  // renderPass;
        0,  // subpass;
        VK_NULL_HANDLE,  // basePipelineHandle;
        -1  // basePipelineIndex;
    };

    VkPipeline pipeline = VK_NULL_HANDLE;
    vkres = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &create_info, vulkan_allocator, &pipeline);
    assert(vkres == VK_SUCCESS);

    // vkDestroyPipelineLayout(device, pipeline_layout, vulkan_allocator);
    vkDestroyShaderModule(device, vertex_shader_module, vulkan_allocator);
    vkDestroyShaderModule(device, fragment_shader_module, vulkan_allocator);
    return pipeline;
}

void f()
{
    std::abort();
}




}  // namespace vkutil
