#pragma once

// Low level vulkan utilities.

#include <volk.h>

#include <vector>
#include <string>


namespace vkutil {
extern VkAllocationCallbacks* vulkan_allocator;
void check_vk_result(VkResult vkres);
std::string version_to_string(uint32_t version);
VkInstance create_vulkan_instance();
std::vector<VkPhysicalDevice> get_physical_devices(VkInstance instance);
void dump_physical_device(VkPhysicalDevice physical_device);
VkPhysicalDeviceMemoryProperties get_physical_device_memory_properties(VkPhysicalDevice physical_device);
VkSurfaceKHR create_win32_surface(VkInstance instance, HINSTANCE hinstance, HWND hwnd);
std::pair<uint32_t, uint32_t> select_graphics_present_queue_family(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
VkDevice create_device(VkPhysicalDevice physical_device, uint32_t queue_family_index);
VkQueue get_device_queue(VkDevice device, uint32_t queue_family_index);
void dump_available_extensions();
VkPresentModeKHR select_present_mode(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
VkSurfaceFormatKHR select_surface_format(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
VkFormat select_depth_format(VkPhysicalDevice physical_device);
std::pair<VkSwapchainKHR, VkExtent2D> create_swapchain(VkPhysicalDevice physical_device,
	VkDevice device, VkSurfaceKHR surface,
	VkPresentModeKHR present_mode, VkSurfaceFormatKHR surface_format,
	VkExtent2D hint_extent,
	VkSwapchainKHR old_swapchain);
std::vector<VkImage> get_swapchain_images(VkDevice device, VkSwapchainKHR swapchain);
VkImageView create_image_view(VkDevice device, VkSurfaceFormatKHR surface_format, VkImage image);
VkCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index);
VkCommandBuffer create_command_buffer(VkDevice device, VkCommandPool command_pool);
VkSemaphore create_semaphore(VkDevice device);
VkFence create_fence(VkDevice device);
VkFramebuffer create_framebuffer(VkDevice device, uint32_t width, uint32_t height,
	VkRenderPass render_pass, std::vector<VkImageView>& image_views);
void destroy_framebuffer(VkDevice device, VkFramebuffer framebuffer);
VkRenderPass create_render_pass(VkDevice device, VkSurfaceFormatKHR surface_format, VkFormat depth_format);
VkRenderPass create_depth_render_pass(VkDevice device, VkFormat depth_format);
VkDescriptorSetLayout create_descriptor_set_layout(VkDevice device);
void destroy_descriptor_set_layout(VkDevice device, VkDescriptorSetLayout descriptor_set_layout);
VkDescriptorPool create_descriptor_pool(VkDevice device, bool enable_free);
void reset_descriptor_pool(VkDevice device, VkDescriptorPool descriptor_pool);
void destroy_descriptor_pool(VkDevice device, VkDescriptorPool descriptor_pool);
VkDescriptorSet create_descriptor_set(VkDevice device,
	VkDescriptorPool descriptor_pool,
	VkDescriptorSetLayout descriptor_set_layout
);
void free_descriptor_sets(VkDevice device, VkDescriptorPool descriptor_pool, const std::vector<VkDescriptorSet>& descriptor_sets);
void destroy_pipeline(VkDevice device, VkPipeline pipeline);
std::pair<VkBuffer, VkDeviceMemory> create_staging_buffer(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkDeviceSize buffer_size);
std::pair<VkBuffer, VkDeviceMemory> create_staging_buffer_coherent(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkDeviceSize buffer_size);
std::pair<VkBuffer, VkDeviceMemory> create_vertex_buffer(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, uint8_t* buffer_data, std::size_t buffer_size);
std::pair<VkBuffer, VkDeviceMemory> create_index_buffer(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, uint8_t* buffer_data, std::size_t buffer_size);
std::pair<VkImage, VkDeviceMemory> create_texture(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, uint32_t width, uint32_t height, uint8_t* buffer_data, std::size_t buffer_size);
VkImageView create_image_view_texture(VkDevice device, VkImage image);
VkSampler create_sampler(VkDevice device);
VkSampler create_depth_sampler(VkDevice device);
VkSampler create_shadow_sampler(VkDevice device);
VkImage create_image_cubemap(VkDevice device, uint32_t width, uint32_t height);
std::pair<VkImage, VkDeviceMemory> create_texture_cubemap(const VkPhysicalDeviceMemoryProperties& memory_properties,
	VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, uint32_t width, 
	uint32_t height, uint8_t* buffer_data, std::size_t buffer_size);
VkImageView create_image_view_texture_cubemap(VkDevice device, VkImage image);
std::pair<VkImage, VkDeviceMemory> create_texture_depth(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device, VkQueue queue, VkCommandBuffer command_buffer, VkFormat format, uint32_t width, uint32_t height, uint32_t array_layers, bool sampled);
VkImageView create_image_view_texture_depth(VkDevice device, VkImage image, VkFormat format, bool use_depth, bool use_stencil, uint32_t base_array_layer, uint32_t layer_count);
std::pair<VkBuffer, VkDeviceMemory> create_uniform_buffer(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device,
	VkQueue queue, VkCommandBuffer command_buffer,
	uint8_t* buffer_data, VkDeviceSize buffer_size);
std::pair<VkBuffer, VkDeviceMemory> create_uniform_buffer_coherent(const VkPhysicalDeviceMemoryProperties& memory_properties, VkDevice device,
	VkQueue queue, VkCommandBuffer command_buffer,
	uint8_t* buffer_data, VkDeviceSize buffer_size);
void update_descriptor_set(VkDevice device, VkDescriptorSet descriptor_set, VkSampler sampler, VkImageView image_view, VkBuffer uniform_buffer, VkDeviceSize uniform_buffer_size);

VkDescriptorSetLayout create_descriptor_set_layout(
	VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& layout_bindings
);
VkPipelineLayout create_pipeline_layout(
	VkDevice device, const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts,
	std::vector<VkPushConstantRange>& push_constant_ranges
);
void destroy_pipeline_layout(VkDevice device, VkPipelineLayout pipeline_layout);
VkPipeline create_pipeline(VkDevice device, VkRenderPass render_pass,
	uint32_t width, uint32_t height,
	std::string filename_vert_spv, std::string filename_frag_spv,
	VkPipelineVertexInputStateCreateInfo vertex_input_state,
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state,
	VkPipelineLayout pipeline_layout,
	VkCullModeFlags cull_model_flags
);


}  // namespace vkutil
