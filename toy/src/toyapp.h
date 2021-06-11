#pragma once

#include "window.h"
#include "vulkan_context.h"
#include "image_util.h"
#include "basic_shapes.h"
#include "basic_pipeline.h"
#include "mesh.h"
#include "material.h"
#include "texture.h"
#include "texture_cubemap.h"
#include "camera.h"
#include "camera_controller.h"
#include "geometry_builder.h"
#include "geometry_mesh.h"
#include "vertex_mesh.h"
#include "text_builder.h"
#include "color_util.h"
#include "resource_manager.h"
#include "timer_manager.h"
#include "input_manager.h"
#include "camera_manager.h"
#include "render_manager.h"


class App {
public:
	App();

	App(const App&) = delete;
	App& operator=(const App&) = delete;

	void startup(VulkanContext& ctx, Window* window);

	void shutdown();

	void update();

	void render(VkCommandBuffer command_buffer);

	void on_resize(uint32_t width, uint32_t height);

	bool is_mouse_down(uint32_t mouse_button);

	void on_key_down(uint32_t key);

	void on_key_up(uint32_t key);

	void on_mouse_down(uint32_t mouse_button, uint32_t x, uint32_t y);

	void on_mouse_up(uint32_t mouse_button, uint32_t x, uint32_t y);

	void on_mouse_move(uint32_t x, uint32_t y);

	void on_mouse_wheel(float wheel_delta, uint32_t x, uint32_t y);

	void set_background_color(glm::vec3 color);

	glm::vec2 get_view_size();

public:
	VulkanContext* ctxptr;
	Window* m_window;

	ResourceManager resource_manager;

	BasicPipeline pipeline;
	BasicPipeline pipeline_lines;
	BasicPipeline pipeline_skybox;
	BasicPipeline pipeline_text;

	double timestamp = 0.0;

	glm::vec3 background_color{ 1.0f, 1.0f, 1.0f };

	VkDescriptorSet descriptor_set_model;
	VkBuffer model_uniform_buffer;
	VkDeviceMemory model_uniform_memory;

	VkDescriptorSet descriptor_set_frame;
	VkDescriptorSet descriptor_set_frame2;
	VkBuffer frame_uniform_buffer;
	VkDeviceMemory frame_uniform_memory;
	VkBuffer frame_uniform_buffer2;
	VkDeviceMemory frame_uniform_memory2;

	Texture texture;
	TextureCubemap texture_cubemap;
	Texture texture_ascii;
	Mesh mesh;
	GeometryMesh mesh2;
	Mesh mesh3;
	VertexMesh mesh_text;
	Material material;
	Material material_cubemap;
	Material material_text;
	std::shared_ptr<Mesh> p_mesh;
	std::shared_ptr<Material> p_material;
	glm::vec3 old_translation;

	FrameUniforms frame_uniform;
	FrameUniforms frame_uniform2;
	ModelUniforms model_uniform;

	GeometryBuilder geometry_builder;
	TimedGeometryBuilder timed_geometry_builder;
	TextBuilder text_builder;
	TimedTextBuilder timed_text_builder;

	KeyDownCallback script_on_key_down;
	TimerManager timer_manager;
	float delta_time;
	InputManager input_manager;
	CameraManager camera_manager;
	RenderManager render_manager;
};
