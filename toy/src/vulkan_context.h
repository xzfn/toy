#pragma once

// Vulkan context.

#include <volk.h>
#include <vector>
#include <utility>
#include <functional>

#include <glm/mat4x4.hpp>

#include "vulkan_resources.h"

class Mesh;
class GeometryMesh;
class Material;


struct VulkanBasic {
	VkInstance instance;
	VkPhysicalDevice physical_device;
	VkSurfaceKHR surface;
	uint32_t queue_family_index;
	VkDevice device;
	VkQueue queue;
	VkPresentModeKHR present_mode;
	VkSurfaceFormatKHR surface_format;
	VkFormat depth_format;
	VkImage depth_image;
	VkImageView depth_image_view;
	VkDeviceMemory depth_memory;
	VkSwapchainKHR swapchain;
	VkExtent2D window_extent;
	VkExtent2D extent;
	VkCommandPool command_pool;
	VkCommandBuffer work_command_buffer;
	VkDescriptorPool descriptor_pool;
	VkRenderPass render_pass;
	VkRenderPass depth_render_pass;
	uint32_t image_count;
	uint32_t frame_index;
};

struct VulkanFrame {
	VkSemaphore image_available_semaphore;
	VkSemaphore rendering_finished_semaphore;
	VkFence fence;
	std::vector<VulkanBuffer> vulkan_buffers;
	std::vector<VkDescriptorSet> vulkan_descriptor_sets;
};

struct VulkanSwapImage {
	VkImage image;
	VkImageView image_view;
	VkFramebuffer framebuffer;
	VkCommandBuffer command_buffer;
};

struct VulkanFeature {
	VkPhysicalDeviceMemoryProperties memory_properties;
};

class VulkanContext {
public:
	const uint32_t frame_count = 2;

public:
	VulkanContext(HINSTANCE hinstance, HWND hwnd, uint32_t width, uint32_t height);
	~VulkanContext();
	VulkanContext(const VulkanContext&) = delete;
	VulkanContext& operator=(const VulkanContext&) = delete;

public:
	void resize(uint32_t width, uint32_t height);
	std::pair<VkBuffer, VkDeviceMemory> create_vertex_buffer(uint8_t* buffer_data, std::size_t buffer_size);
	std::pair<VkBuffer, VkDeviceMemory> create_index_buffer(uint8_t* buffer_data, std::size_t buffer_size);
	std::pair<VkImage, VkDeviceMemory> create_texture(uint32_t width, uint32_t height,
		uint8_t* buffer_data, std::size_t buffer_size);
	std::pair<VkImage, VkDeviceMemory> create_texture_cubemap(uint32_t width, uint32_t height,
		uint8_t* buffer_data, std::size_t buffer_size);
	std::pair<VkImage, VkDeviceMemory> create_texture_depth(uint32_t width, uint32_t height, uint32_t array_layers, bool sampled);
	VkImageView create_image_view_texture(VkImage image);
	VkImageView create_image_view_texture_cubemap(VkImage image);
	VkImageView create_image_view_texture_depth(VkImage image, bool use_depth, bool use_stencil, uint32_t base_array_layer, uint32_t layer_count);
	VkSampler create_sampler();
	VkSampler create_depth_sampler();
	void destroy_buffer(VkBuffer buffer);
	void free_memory(VkDeviceMemory memory);
	void destroy_buffer_and_memory(VkBuffer buffer, VkDeviceMemory memory);
	void destroy_vulkan_buffer(VulkanBuffer vulkan_buffer);
	void destroy_vulkan_buffer_immediately(VulkanBuffer vulkan_buffer);
	void destroy_vulkan_descriptor_sets(const std::vector<VkDescriptorSet>& descriptor_sets);
	void destroy_image_view(VkImageView image_view);
	void destroy_image(VkImage image);
	void destroy_vulkan_image(VulkanImage vulkan_image);
	void destroy_sampler(VkSampler sampler);
	VkFramebuffer create_framebuffer(uint32_t width, uint32_t height,
		VkRenderPass render_pass, std::vector<VkImageView>& image_views);
	std::pair<VkBuffer, VkDeviceMemory> create_uniform_buffer(uint8_t* buffer_data, std::size_t buffer_size);
	std::pair<VkBuffer, VkDeviceMemory> create_uniform_buffer_coherent(uint8_t* buffer_data, std::size_t buffer_size);
	VkDescriptorSet create_descriptor_set(VkDescriptorSetLayout descriptor_set_layout);
	void update_descriptor_set(VkDescriptorSet descriptor_set, VkSampler sampler, VkImageView image_view, VkBuffer uniform_buffer, std::size_t uniform_buffer_size);
	void free_descriptor_sets(const std::vector<VkDescriptorSet>& descriptor_sets);
	void render(VkClearColorValue clear_color,
		std::function<void(VkCommandBuffer command_buffer)> depth_pass_callback,
		std::function<void(VkCommandBuffer command_buffer)> render_callback
		);
	void device_wait_idle();

private:
	void init_vulkan(HINSTANCE hinstance, HWND hwnd, uint32_t width, uint32_t height);
	void init_vulkan_instance();
	void init_physical_device();
	void init_feature();
	void init_surface(HINSTANCE hinstance, HWND hwnd);
	void init_queue_family_index();
	void init_device();
	void init_queue();
	void init_present_mode();
	void init_surface_format();
	void init_depth_format();
	void init_depth();
	void init_depth_render_pass();
	void init_render_pass();
	void init_frames();
	void init_swapchain(uint32_t width, uint32_t height);
	void init_command_pool();
	void init_swapchain_images();
	void init_descriptor_pool();

	void recreate_swapchain();

	void destroy_vulkan();

public:
	VulkanBasic basic{ 0 };
	VulkanFeature feature{ 0 };
	std::vector<VulkanFrame> frames;
	std::vector<VulkanSwapImage> swap_images;
};
