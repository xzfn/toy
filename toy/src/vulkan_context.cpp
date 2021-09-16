
#include "vulkan_context.h"

#include <cassert>
#include <iostream>
#include <array>

#include <glm/gtc/type_ptr.hpp>

#include "vulkan_util.h"
#include "vulkan_helper.h"
#include "vertex_format.h"

#include "material.h"


VulkanContext::VulkanContext(HINSTANCE hinstance, HWND hwnd, uint32_t width, uint32_t height)
{
	init_vulkan(hinstance, hwnd, width, height);
}

VulkanContext::~VulkanContext()
{
	destroy_vulkan();
}

void VulkanContext::resize(uint32_t width, uint32_t height)
{
	basic.window_extent.width = width;
	basic.window_extent.height = height;
	//recreate_swapchain();
}

std::pair<VkBuffer, VkDeviceMemory> VulkanContext::create_vertex_buffer(uint8_t* buffer_data, std::size_t buffer_size)
{
	return vkutil::create_vertex_buffer(
		feature.memory_properties,
		basic.device,
		basic.queue,
		basic.work_command_buffer,
		buffer_data,
		buffer_size
	);
}

std::pair<VkBuffer, VkDeviceMemory> VulkanContext::create_index_buffer(uint8_t* buffer_data, std::size_t buffer_size)
{
	return vkutil::create_index_buffer(
		feature.memory_properties,
		basic.device,
		basic.queue,
		basic.work_command_buffer,
		buffer_data,
		buffer_size
	);
}

std::pair<VkImage, VkDeviceMemory> VulkanContext::create_texture(uint32_t width, uint32_t height, uint8_t* buffer_data, std::size_t buffer_size)
{
	return vkutil::create_texture(
		feature.memory_properties,
		basic.device,
		basic.queue,
		basic.work_command_buffer,
		width, height,
		buffer_data, buffer_size
	);
}

std::pair<VkImage, VkDeviceMemory> VulkanContext::create_texture_cubemap(uint32_t width, uint32_t height, uint8_t* buffer_data, std::size_t buffer_size)
{
	return vkutil::create_texture_cubemap(
		feature.memory_properties,
		basic.device,
		basic.queue,
		basic.work_command_buffer,
		width, height,
		buffer_data, buffer_size
	);
}

std::pair<VkImage, VkDeviceMemory> VulkanContext::create_texture_depth(uint32_t width, uint32_t height, uint32_t array_layers, bool sampled)
{
	return vkutil::create_texture_depth(
		feature.memory_properties,
		basic.device,
		basic.queue,
		basic.work_command_buffer,
		basic.depth_format,
		width, height,
		array_layers,
		sampled
	);
}

VkImageView VulkanContext::create_image_view_texture(VkImage image)
{
	return vkutil::create_image_view_texture(basic.device, image);
}

VkImageView VulkanContext::create_image_view_texture_cubemap(VkImage image)
{
	return vkutil::create_image_view_texture_cubemap(basic.device, image);
}

VkImageView VulkanContext::create_image_view_texture_depth(VkImage image, bool use_depth, bool use_stencil, uint32_t base_array_layer, uint32_t layer_count)
{
	return vkutil::create_image_view_texture_depth(
		basic.device, image, basic.depth_format, use_depth, use_stencil, base_array_layer, layer_count
	);
}

VkSampler VulkanContext::create_sampler()
{
	return vkutil::create_sampler(basic.device);
}

VkSampler VulkanContext::create_depth_sampler()
{
	return vkutil::create_depth_sampler(basic.device);
}

VkSampler VulkanContext::create_shadow_sampler()
{
	return vkutil::create_shadow_sampler(basic.device);
}

void VulkanContext::destroy_buffer(VkBuffer buffer)
{
	vkDestroyBuffer(basic.device, buffer, vkutil::vulkan_allocator);
}

void VulkanContext::free_memory(VkDeviceMemory memory)
{
	vkFreeMemory(basic.device, memory, vkutil::vulkan_allocator);
}

void VulkanContext::destroy_buffer_and_memory(VkBuffer buffer, VkDeviceMemory memory)
{
	destroy_buffer(buffer);
	free_memory(memory);
}

void VulkanContext::destroy_vulkan_buffer(VulkanBuffer vulkan_buffer)
{
	auto& frame = frames[basic.frame_index];
	frame.vulkan_buffers.push_back(vulkan_buffer);
}

void VulkanContext::destroy_vulkan_buffer_immediately(VulkanBuffer vulkan_buffer)
{
	destroy_buffer_and_memory(vulkan_buffer.buffer, vulkan_buffer.memory);
}

void VulkanContext::destroy_image_view(VkImageView image_view)
{
	vkDestroyImageView(basic.device, image_view, vkutil::vulkan_allocator);
}

void VulkanContext::destroy_image(VkImage image)
{
	vkDestroyImage(basic.device, image, vkutil::vulkan_allocator);
}

void VulkanContext::destroy_vulkan_image(VulkanImage vulkan_image)
{
	destroy_image(vulkan_image.image);
	free_memory(vulkan_image.memory);
}

void VulkanContext::destroy_sampler(VkSampler sampler)
{
	vkDestroySampler(basic.device, sampler, vkutil::vulkan_allocator);
}

VkFramebuffer VulkanContext::create_framebuffer(uint32_t width, uint32_t height, VkRenderPass render_pass, std::vector<VkImageView>& image_views)
{
	return vkutil::create_framebuffer(
		basic.device,
		width,
		height,
		render_pass,
		image_views
	);
}

void VulkanContext::destroy_framebuffer(VkFramebuffer framebuffer)
{
	return vkutil::destroy_framebuffer(
		basic.device,
		framebuffer
	);
}

std::pair<VkBuffer, VkDeviceMemory> VulkanContext::create_uniform_buffer(uint8_t* buffer_data, std::size_t buffer_size)
{
	return vkutil::create_uniform_buffer(
		feature.memory_properties,
		basic.device,
		basic.queue,
		basic.work_command_buffer,
		buffer_data, buffer_size
	);
}

void VulkanContext::update_descriptor_set(VkDescriptorSet descriptor_set, VkSampler sampler, VkImageView image_view, VkBuffer uniform_buffer, std::size_t uniform_buffer_size)
{
	vkutil::update_descriptor_set(
		basic.device,
		descriptor_set,
		sampler,
		image_view,
		uniform_buffer,
		uniform_buffer_size
	);
}

void VulkanContext::free_descriptor_sets(const std::vector<VkDescriptorSet>& descriptor_sets)
{
	vkutil::free_descriptor_sets(basic.device, basic.descriptor_pool, descriptor_sets);
}

void VulkanContext::reset_descriptor_pool(VkDescriptorPool descriptor_pool)
{
	vkutil::reset_descriptor_pool(basic.device, descriptor_pool);
}

void VulkanContext::destroy_descriptor_pool(VkDescriptorPool descriptor_pool)
{
	vkutil::destroy_descriptor_pool(basic.device, descriptor_pool);
}

void VulkanContext::destroy_vulkan_descriptor_sets(const std::vector<VkDescriptorSet>& descriptor_sets)
{
	auto& frame = frames[basic.frame_index];
	auto& v = frame.vulkan_descriptor_sets;
	v.insert(v.end(), descriptor_sets.begin(), descriptor_sets.end());
}

void VulkanContext::render(VkClearColorValue clear_color,
	std::function<void(VkCommandBuffer command_buffer)> depth_pass_callback,
	std::function<void(VkCommandBuffer command_buffer)> render_callback
)
{
	VkResult vkres;
	uint64_t timeout = UINT64_MAX;
	uint32_t image_index = 0;

	uint32_t width = basic.extent.width;
	uint32_t height = basic.extent.height;

	basic.frame_index = (basic.frame_index + 1) % frame_count;
	VulkanFrame& frame = frames[basic.frame_index];
	vkres = vkWaitForFences(basic.device, 1, &frame.fence, VK_FALSE, 0xffffff);
	vkutil::check_vk_result(vkres);
	vkResetFences(basic.device, 1, &frame.fence);

	for (auto& vulkan_buffer : frame.vulkan_buffers) {
		destroy_vulkan_buffer_immediately(vulkan_buffer);
	}
	frame.vulkan_buffers.clear();
	free_descriptor_sets(frame.vulkan_descriptor_sets);
	frame.vulkan_descriptor_sets.clear();
	reset_descriptor_pool(frame.descriptor_pool);

	vkres = vkAcquireNextImageKHR(
		basic.device,
		basic.swapchain,
		timeout,
		frame.image_available_semaphore,
		VK_NULL_HANDLE,
		&image_index
	);
	switch (vkres) {
	case VK_SUCCESS:
		break;
	case VK_SUBOPTIMAL_KHR:
		std::cout << "WARN vkAcquireNextImageKHR suboptimal\n";
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		std::cout << "WARN vkAcquireNextImageKHR out of date\n";
		recreate_swapchain();
		break;
	default:
		assert(false);
		break;
	}

	VulkanSwapImage swap_image = swap_images[image_index];

	// render
	VkCommandBuffer command_buffer = swap_image.command_buffer;
	vkres = vkResetCommandBuffer(command_buffer, 0);
	vkutil::check_vk_result(vkres);

	VkCommandBufferBeginInfo begin_info = {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,  // sType;
		nullptr,  // pNext;
		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,  // flags;
		nullptr  // pInheritanceInfo;
	};
	vkBeginCommandBuffer(command_buffer, &begin_info);

	// shadow
	depth_pass_callback(command_buffer);

	// render
	VkRect2D render_area = {
	{
		0, 0
	},
	{
		width, height
	}
	};

	VkClearDepthStencilValue clear_depth_value{
		1.0f,
		0
	};

	std::array<VkClearValue, 2> clear_values;
	clear_values[0].color = clear_color;
	clear_values[1].depthStencil = clear_depth_value;

	VkFramebuffer framebuffer = swap_image.framebuffer;
	VkRenderPassBeginInfo render_pass_begin_info = {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,  // sType;
		nullptr,  // pNext;
		basic.render_pass,  // renderPass;
		framebuffer,  // framebuffer;
		render_area,  // renderArea;
		(uint32_t)clear_values.size(),  // clearValueCount;
		clear_values.data()  // pClearValues;
	};
	vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);


	VkViewport viewport = {
		0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f
	};
	vkhelper::flip_viewport(viewport);

	VkRect2D scissor = {
		{
			0, 0
		},
		{
			width, height
		}
	};
	vkCmdSetViewport(command_buffer, 0, 1, &viewport);
	vkCmdSetScissor(command_buffer, 0, 1, &scissor);

	// -----------------------------
	render_callback(command_buffer);

	// -----
	vkCmdEndRenderPass(command_buffer);

	vkres = vkEndCommandBuffer(command_buffer);
	assert(vkres == VK_SUCCESS);


	VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkSubmitInfo submit_info = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType;
		nullptr,  // pNext;
		1,  // waitSemaphoreCount;
		&frame.image_available_semaphore,  // pWaitSemaphores;
		&wait_dst_stage_mask,  // pWaitDstStageMask;
		1,  // commandBufferCount;
		&command_buffer,  // pCommandBuffers;
		1,  // signalSemaphoreCount;
		&frame.rendering_finished_semaphore,  // pSignalSemaphores;
	};
	vkres = vkQueueSubmit(basic.queue, 1, &submit_info, frame.fence);
	assert(vkres == VK_SUCCESS);

	VkPresentInfoKHR present_info = {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,  // sType;
		nullptr,  // pNext;
		1,  // waitSemaphoreCount;
		&frame.rendering_finished_semaphore,  // pWaitSemaphores;
		1,  // swapchainCount;
		&basic.swapchain,  // pSwapchains;
		&image_index,  // pImageIndices;
		nullptr  // pResults;
	};
	vkres = vkQueuePresentKHR(basic.queue, &present_info);
	switch (vkres) {
	case VK_SUCCESS:
		break;
	case VK_SUBOPTIMAL_KHR:
		std::cout << "QueuePresent suboptimal\n";
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		return recreate_swapchain();
	default:
		assert(false);
		break;
	}

}

void VulkanContext::device_wait_idle()
{
	vkDeviceWaitIdle(basic.device);
}

std::pair<VkBuffer, VkDeviceMemory> VulkanContext::create_uniform_buffer_coherent(uint8_t* buffer_data, std::size_t buffer_size)
{
	return vkutil::create_uniform_buffer_coherent(
		feature.memory_properties,
		basic.device,
		basic.queue,
		basic.work_command_buffer,
		buffer_data, buffer_size
	);
}

void VulkanContext::init_vulkan(HINSTANCE hinstance, HWND hwnd, uint32_t width, uint32_t height)
{
	VkResult vkres;
	vkres = volkInitialize();
	vkutil::check_vk_result(vkres);

	basic.window_extent.width = width;
	basic.window_extent.height = height;

	init_vulkan_instance();

	volkLoadInstanceOnly(basic.instance);

	init_physical_device();
	init_feature();

	init_surface(hinstance, hwnd);

	init_queue_family_index();

	init_device();

	volkLoadDevice(basic.device);

	init_queue();

	vkutil::dump_available_extensions();

	init_present_mode();

	init_surface_format();

	init_depth_format();

	init_depth();

	init_depth_render_pass();

	init_render_pass();

	init_command_pool();

	init_frames();

	init_swapchain(width, height);
	init_swapchain_images();

	init_descriptor_pool();

}

VkDescriptorSet VulkanContext::create_descriptor_set(VkDescriptorSetLayout descriptor_set_layout)
{
	return vkutil::create_descriptor_set(basic.device, basic.descriptor_pool, descriptor_set_layout);
}

VkDescriptorSet VulkanContext::create_descriptor_set_transient(VkDescriptorSetLayout descriptor_set_layout)
{
	return vkutil::create_descriptor_set(basic.device, frames[basic.frame_index].descriptor_pool, descriptor_set_layout);
}

void VulkanContext::init_vulkan_instance()
{
	basic.instance = vkutil::create_vulkan_instance();
}

void VulkanContext::init_physical_device()
{
	auto physical_devices = vkutil::get_physical_devices(basic.instance);
	assert(physical_devices.size() > 0);  // no vulkan device
	for (auto& physical_device : physical_devices) {
		vkutil::dump_physical_device(physical_device);
	}
	// use first device
	basic.physical_device = physical_devices[0];
}

void VulkanContext::init_feature()
{
	feature.memory_properties = vkutil::get_physical_device_memory_properties(basic.physical_device);
}

void VulkanContext::init_surface(HINSTANCE hinstance, HWND hwnd)
{
	basic.surface = vkutil::create_win32_surface(basic.instance, hinstance, hwnd);
}

void VulkanContext::init_queue_family_index()
{
	auto graphics_and_present_index = vkutil::select_graphics_present_queue_family(basic.physical_device, basic.surface);
	uint32_t graphics_queue_family_index = graphics_and_present_index.first;
	uint32_t present_queue_family_index = graphics_and_present_index.second;
	if (graphics_queue_family_index == present_queue_family_index) {
		// same queue index, this is supported
		basic.queue_family_index = graphics_queue_family_index;
	}
	else {
		std::cout << "ERROR different graphics and present queue family index not supported\n";
		std::exit(-1);
	}
}

void VulkanContext::init_device() {
	basic.device = vkutil::create_device(basic.physical_device, basic.queue_family_index);
}

void VulkanContext::init_queue()
{
	basic.queue = vkutil::get_device_queue(basic.device, basic.queue_family_index);
}

void VulkanContext::init_present_mode()
{
	basic.present_mode = vkutil::select_present_mode(basic.physical_device, basic.surface);
}

void VulkanContext::init_surface_format()
{
	basic.surface_format = vkutil::select_surface_format(basic.physical_device, basic.surface);
}

void VulkanContext::init_depth_format()
{
	basic.depth_format = vkutil::select_depth_format(basic.physical_device);
}

void VulkanContext::init_depth()
{
	if (basic.depth_image_view) {
		vkDestroyImageView(basic.device, basic.depth_image_view, vkutil::vulkan_allocator);
		vkDestroyImage(basic.device, basic.depth_image, vkutil::vulkan_allocator);
		vkFreeMemory(basic.device, basic.depth_memory, vkutil::vulkan_allocator);
	}
	
	std::pair<VkImage, VkDeviceMemory> image_and_memory = create_texture_depth(basic.window_extent.width, basic.window_extent.height, 1, false);
	VkImage depth_image = image_and_memory.first;
	VkImageView depth_image_view = create_image_view_texture_depth(depth_image, true, true, 0, 1);
	basic.depth_image = depth_image;
	basic.depth_image_view = depth_image_view;
	basic.depth_memory = image_and_memory.second;
}

void VulkanContext::init_depth_render_pass()
{
	basic.depth_render_pass = vkutil::create_depth_render_pass(basic.device, basic.depth_format);
}

void VulkanContext::init_render_pass()
{
	basic.render_pass = vkutil::create_render_pass(basic.device, basic.surface_format, basic.depth_format);
}

void VulkanContext::init_frames() {
	// frames
	frames.resize(frame_count);
	for (uint32_t i = 0; i < frame_count; ++i) {
		VulkanFrame& frame = frames[i];
		frame.image_available_semaphore = vkutil::create_semaphore(basic.device);
		frame.rendering_finished_semaphore = vkutil::create_semaphore(basic.device);
		frame.fence = vkutil::create_fence(basic.device);
		frame.descriptor_pool = vkutil::create_descriptor_pool(basic.device, false);
	}
}

void VulkanContext::init_swapchain(uint32_t width, uint32_t height)
{
	VkExtent2D hint_extent{
		width,
		height
	};
	auto old_swapchain = basic.swapchain;
	auto swapchain_and_extent = vkutil::create_swapchain(
		basic.physical_device,
		basic.device,
		basic.surface,
		basic.present_mode,
		basic.surface_format,
		hint_extent,
		old_swapchain
	);

	if (swapchain_and_extent.first == VK_NULL_HANDLE) {
		return;
	}
	basic.swapchain = swapchain_and_extent.first;
	basic.extent = swapchain_and_extent.second;
}

void VulkanContext::init_command_pool()
{
	basic.command_pool = vkutil::create_command_pool(basic.device, basic.queue_family_index);
	basic.work_command_buffer = vkutil::create_command_buffer(basic.device,  basic.command_pool);
}

void VulkanContext::init_swapchain_images()
{
	auto device = basic.device;

	if (swap_images.size() > 0) {
		for (uint32_t i = 0; i < swap_images.size(); ++i) {
			VulkanSwapImage& swap_image = swap_images[i];
			destroy_framebuffer(swap_image.framebuffer);
			vkFreeCommandBuffers(device, basic.command_pool, 1, &swap_image.command_buffer);
			destroy_image_view(swap_image.image_view);
			//destroy_image(swap_image.image);  // image belong to swapchain
		}
		swap_images.clear();
	}

	auto images = vkutil::get_swapchain_images(basic.device, basic.swapchain);
	uint32_t image_count = (uint32_t)images.size();

	swap_images.resize(image_count);
	for (uint32_t i = 0; i < image_count; ++i) {
		VulkanSwapImage& swap_image = swap_images[i];
		VkImage image = images[i];
		VkImageView image_view = vkutil::create_image_view(basic.device, basic.surface_format, image);

		std::vector<VkImageView> image_views{
			image_view, basic.depth_image_view
		};
		VkFramebuffer framebuffer = vkutil::create_framebuffer(basic.device, basic.extent.width, basic.extent.height, basic.render_pass, image_views);
		VkCommandBuffer command_buffer = vkutil::create_command_buffer(basic.device, basic.command_pool);

		swap_image.image = image;
		swap_image.image_view = image_view;
		swap_image.framebuffer = framebuffer;
		swap_image.command_buffer = command_buffer;
	}
}


void VulkanContext::init_descriptor_pool()
{
	basic.descriptor_pool = vkutil::create_descriptor_pool(basic.device, true);
}

void VulkanContext::recreate_swapchain()
{
	std::cout << "recreate_swapchain\n";
	device_wait_idle();
	init_swapchain(basic.window_extent.width, basic.window_extent.height);
	init_depth();
	init_swapchain_images();
}

void VulkanContext::destroy_vulkan()
{
	auto device = basic.device;
	auto vulkan_allocator = vkutil::vulkan_allocator;
	vkDeviceWaitIdle(device);

	vkFreeCommandBuffers(device, basic.command_pool, 1, &basic.work_command_buffer);

	for (uint32_t i = 0; i < frames.size(); ++i) {
		VulkanFrame& frame = frames[i];
		vkDestroySemaphore(device, frame.image_available_semaphore, vulkan_allocator);
		vkDestroySemaphore(device, frame.rendering_finished_semaphore, vulkan_allocator);
		vkDestroyFence(device, frame.fence, vulkan_allocator);
		for (auto& vulkan_buffer : frame.vulkan_buffers) {
			destroy_vulkan_buffer_immediately(vulkan_buffer);
		}
		frame.vulkan_buffers.clear();
		free_descriptor_sets(frame.vulkan_descriptor_sets);
		frame.vulkan_descriptor_sets.clear();
		reset_descriptor_pool(frame.descriptor_pool);
		destroy_descriptor_pool(frame.descriptor_pool);
	}

	for (uint32_t i = 0; i < swap_images.size(); ++i) {
		VulkanSwapImage& swap_image = swap_images[i];
		destroy_framebuffer(swap_image.framebuffer);
		vkFreeCommandBuffers(device, basic.command_pool, 1, &swap_image.command_buffer);
		destroy_image_view(swap_image.image_view);
		//destroy_image(swap_image.image);  // image belong to swapchain
	}

	destroy_image_view(basic.depth_image_view);
	destroy_image(basic.depth_image);
	free_memory(basic.depth_memory);

	vkDestroyCommandPool(device, basic.command_pool, vulkan_allocator);

	destroy_descriptor_pool(basic.descriptor_pool);
	vkDestroyRenderPass(device, basic.render_pass, vulkan_allocator);
	vkDestroyRenderPass(device, basic.depth_render_pass, vulkan_allocator);

	vkDestroySwapchainKHR(device, basic.swapchain, vulkan_allocator);
	vkDestroyDevice(device, vulkan_allocator);
	vkDestroySurfaceKHR(basic.instance, basic.surface, vulkan_allocator);
	vkDestroyInstance(basic.instance, vulkan_allocator);
}
