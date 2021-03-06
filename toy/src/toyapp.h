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
#include "light_manager.h"
#include "shadow_manager.h"


class App {
public:
	App();

	App(const App&) = delete;
	App& operator=(const App&) = delete;

	void startup(VulkanContext& ctx, Window* window);

	void shutdown();

	void update();

	void depth_pass(VkCommandBuffer command_buffer);

	void pre_render(VkCommandBuffer command_buffer);

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

	VulkanContext& get_ctx();

	glm::vec2 get_view_size();

public:
	VulkanContext* ctxptr;
	Window* m_window;

	ResourceManager resource_manager;
	ShadowManager shadow_manager;

	BasicPipeline pipeline;
	BasicPipeline pipeline_lines;
	BasicPipeline pipeline_skybox;
	BasicPipeline pipeline_text;
	BasicPipeline pipeline_depth;

	std::shared_ptr<Material> material_lines;

	double timestamp = 0.0;

	glm::vec3 background_color{ 1.0f, 1.0f, 1.0f };

	VkDescriptorSet descriptor_set_frame;
	VkDescriptorSet descriptor_set_light;
	VkDescriptorSet descriptor_set_shadow;

	Texture texture;
	TextureCubemap texture_cubemap;
	Texture texture_ascii;

	Mesh mesh_cube;
	Material material_cubemap;
	Material material_text;

	FrameUniforms frame_uniform;

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
	LightManager light_manager;

};
