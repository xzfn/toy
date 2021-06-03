
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

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
#include "color_util.h"
#include "resource_manager.h"


struct VertexData {
	float x, y, z, w;
	float u, v;
};

class App {
public:
	App() { }

	App(const App&) = delete;
	App& operator=(const App&) = delete;

	void startup(VulkanContext& ctx, Window* window) {
		ctxptr = &ctx;
		m_window = window;

		resource_manager.add_resource_directory("../../toy");
		resource_manager.add_resource_directory("../../toy_generated");

		window->set_resize_callback([this](int width, int height) {
			this->on_resize(width, height);
		});

		window->set_update_callback([this]() {
			this->update();
		});

		window->set_key_down_callback([this](uint32_t key) {
			this->on_key_down(key);
		});
		window->set_key_up_callback([this](uint32_t key) {
			this->on_key_up(key);
		});

		window->set_mouse_down_callback([this](uint32_t mouse_button, uint32_t x, uint32_t y) {
			this->on_mouse_down(mouse_button, x, y);
		});
		window->set_mouse_up_callback([this](uint32_t mouse_button, uint32_t x, uint32_t y) {
			this->on_mouse_up(mouse_button, x, y);
		});

		window->set_mouse_move_callback([this](uint32_t x, uint32_t y) {
			this->on_mouse_move(x, y);
		});

		float axis_length = 5.0f;
		geometry_builder.add_line(VEC3_ZERO, VEC3_X * axis_length, COLOR_RED);
		geometry_builder.add_line(VEC3_ZERO, VEC3_Y * axis_length, COLOR_GREEN);
		geometry_builder.add_line(VEC3_ZERO, VEC3_Z * axis_length, COLOR_BLUE);
		geometry_builder.add_cube(VEC3_X * axis_length, 1.0f, COLOR_RED);
		geometry_builder.add_cube(VEC3_Y * axis_length, 1.0f, COLOR_GREEN);
		geometry_builder.add_cube(VEC3_Z * axis_length, 1.0f, COLOR_BLUE);
		geometry_builder.add_sphere(VEC3_X * 4.0f, 2.0f, COLOR_WHITE);
		geometry_builder.add_sphere(VEC3_X * 10.0f, 2.0f, COLOR_WHITE);

		mesh2.init_resource(ctx, geometry_builder.build_buffer());


		ShapeData shape_data = generate_unit_cube();
		MeshData mesh_data;
		mesh_data.set_vertices_data(shape_data.positions, shape_data.normals, shape_data.uvs);
		mesh_data.set_indices(shape_data.indices);
		mesh.init_resource(ctx, mesh_data);

		shape_data = generate_cube();
		mesh_data.set_vertices_data(shape_data.positions, shape_data.normals, shape_data.uvs);
		mesh_data.set_indices(shape_data.indices);
		mesh3.init_resource(ctx, mesh_data);


		PipelineDescription desc;
		desc.filename_vert_spv = resource_manager.full_path("shader/basic.vert.spv");
		desc.filename_frag_spv = resource_manager.full_path("shader/basic.frag.spv");
		desc.vertex_format = VertexFormat::PositionNormalUv;
		desc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		desc.cull_model_flags = VK_CULL_MODE_BACK_BIT;
		pipeline.init_pipeline(ctx, desc);

		
		desc.filename_vert_spv = resource_manager.full_path("shader/vertex_color.vert.spv");
		desc.filename_frag_spv = resource_manager.full_path("shader/vertex_color.frag.spv");
		desc.vertex_format = VertexFormat::PositionColor;
		desc.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		desc.cull_model_flags = VK_CULL_MODE_BACK_BIT;
		pipeline_lines.init_pipeline(ctx, desc);


		desc.filename_vert_spv = resource_manager.full_path("shader/skybox.vert.spv");
		desc.filename_frag_spv = resource_manager.full_path("shader/skybox.frag.spv");
		desc.vertex_format = VertexFormat::PositionNormalUv;
		desc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		desc.cull_model_flags = VK_CULL_MODE_FRONT_BIT;
		pipeline_skybox.init_pipeline(ctx, desc);


		imageutil::Image raw_image = imageutil::load_image_force_channels(resource_manager.full_path("resource/cube.png"), 4);
		texture.init(ctx, raw_image);

		material.init(ctx, pipeline, texture);

		std::vector<imageutil::Image> cubemap_raw_images;
		//front, back, up, down, right and left
		std::vector<std::string> skybox_names{
			//"front", "back", "top", "bottom", "right", "left"
			"right", "left", "top", "bottom", "front", "back"
			//"_px", "_nx", "_py", "_ny", "_pz", "_nz"
		};
		for (std::size_t i = 0; i < 6; ++i) {
			std::string filename = std::string("resource/skybox/") + skybox_names[i] + ".jpg";
			filename = resource_manager.full_path(filename);
			cubemap_raw_images.push_back(imageutil::load_image_force_channels(filename, 4));
		}
		texture_cubemap.init(ctx, cubemap_raw_images);
		material_cubemap.init(ctx, pipeline_skybox, texture_cubemap);


		DescriptorSetLayouts pipeline_descriptor_set_layouts = pipeline.ref_descriptor_set_layouts();


		descriptor_set_frame = ctx.create_descriptor_set(pipeline_descriptor_set_layouts.model);
		auto frame_uniform_buffer_and_memory = ctx.create_uniform_buffer_coherent(
			nullptr, sizeof(FrameUniforms));
		frame_uniform_buffer = frame_uniform_buffer_and_memory.first;
		frame_uniform_memory = frame_uniform_buffer_and_memory.second;
		pipeline.update_descriptor_set_frame(descriptor_set_frame, frame_uniform_buffer, sizeof(FrameUniforms));

		descriptor_set_frame2 = ctx.create_descriptor_set(pipeline_descriptor_set_layouts.model);
		frame_uniform_buffer_and_memory = ctx.create_uniform_buffer_coherent(
			nullptr, sizeof(FrameUniforms));
		frame_uniform_buffer2 = frame_uniform_buffer_and_memory.first;
		frame_uniform_memory2 = frame_uniform_buffer_and_memory.second;
		pipeline.update_descriptor_set_frame(descriptor_set_frame2, frame_uniform_buffer2, sizeof(FrameUniforms));


		descriptor_set_model = ctx.create_descriptor_set(pipeline_descriptor_set_layouts.model);
		auto model_uniform_buffer_and_memory= ctx.create_uniform_buffer_coherent(
			nullptr, sizeof(ModelUniforms));
		model_uniform_buffer = model_uniform_buffer_and_memory.first;
		model_uniform_memory = model_uniform_buffer_and_memory.second;
		pipeline.update_descriptor_set_model(descriptor_set_model, model_uniform_buffer, sizeof(ModelUniforms));


		auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
		timestamp = std::chrono::duration<double>(now).count();
	}

	void shutdown() {
		VulkanContext& ctx = *ctxptr;
		ctx.device_wait_idle();

		ctx.destroy_buffer_and_memory(model_uniform_buffer, model_uniform_memory);
		ctx.destroy_buffer_and_memory(frame_uniform_buffer, frame_uniform_memory);
		ctx.destroy_buffer_and_memory(frame_uniform_buffer2, frame_uniform_memory2);

	}

	void update() {
		VulkanContext& ctx = *ctxptr;
		auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
		double new_timestamp = std::chrono::duration<double>(now).count();
		float delta_time = new_timestamp - timestamp;
		timestamp = new_timestamp;

		double integral;
		float fractional = std::modf(timestamp / 10.0f, &integral);
		VkClearColorValue clear_color{
			std::abs(fractional - 0.5f), 0.0, 0.0f, 1.0f
		};

		float theta = timestamp;
		float cos_theta = cosf(theta);
		float sin_theta = sinf(theta);

		float axis_x = 0.0f;
		float axis_y = 0.0f;
		float axis_z = 0.0f;
		if (is_key_down('W')) {
			axis_z += 1.0;
		}
		if (is_key_down('S')) {
			axis_z -= 1.0;
		}
		if (is_key_down('A')) {
			axis_x += 1.0;
		}
		if (is_key_down('D')) {
			axis_x -= 1.0;
		}
		if (is_key_down('E')) {
			axis_y += 1.0;
		}
		if (is_key_down('Q')) {
			axis_y -= 1.0;
		}
		camera_controller.translate(3.0f * delta_time * glm::vec3(-axis_x, axis_y, -axis_z));
		
		auto extent = ctx.basic.window_extent;
		float aspect = 1.0f * extent.width / extent.height;
		camera.set_perspective_param(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
		Transform camera_transform;
		camera_transform.translation = glm::vec3(0.0f, 1.0f, -5.0f);

		camera_transform.rotation = glm::quatLookAt(
			glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		camera_transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);

		camera_transform = camera_controller.get_transform();


		glm::mat4 view = transform_to_mat4(transform_inverse(camera_transform));

		camera.set_view(view);

		glm::mat4 model = glm::rotate(glm::mat4(1.0), theta / 5.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		
		glm::mat4 model2 = glm::rotate(glm::mat4(1.0), theta, glm::vec3(0.0f, 1.0f, 0.0f));

		frame_uniform.view_projection = camera.get_view_projection();
		theta = timestamp * 3.0f;
		cos_theta = cosf(theta);
		sin_theta = sinf(theta);
		frame_uniform.sun_light_direction = glm::vec3(cos_theta, 0.5f, sin_theta);
		model_uniform.model = model;

		MaterialUniforms& material_uniform = material.ref_uniforms();
		material_uniform.base_color = glm::vec3(cos_theta, 1.0f, sin_theta);

		std::vector<VkDescriptorSet> descriptor_sets{
			descriptor_set_frame, descriptor_set_model
		};

		std::vector<VkDescriptorSet> descriptor_sets2{
			descriptor_set_frame  //, descriptor_set_model
		};

		ctx.render(clear_color,
			pipeline.get_pipeline(), pipeline.get_pipeline_layout(),
			pipeline_lines.get_pipeline(), pipeline_lines.get_pipeline_layout(),
			descriptor_sets,
			descriptor_sets2,
			mesh,
			mesh2,
			material,
			frame_uniform_memory, (uint8_t*)&frame_uniform, sizeof(frame_uniform),
			model_uniform_memory, (uint8_t*)&model_uniform, sizeof(model_uniform),
			model2,
			[this](VkCommandBuffer command_buffer) { this->render(command_buffer); }
		);


		timed_geometry_builder.update(delta_time);
	}

	void render(VkCommandBuffer command_buffer) {
		VulkanContext& ctx = *ctxptr;

		glm::mat4 model;

		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_skybox.get_pipeline());
		std::vector<VkDescriptorSet> descriptor_sets3{
			descriptor_set_frame  //, descriptor_set_model
		};
		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_lines.get_pipeline_layout(), 0, descriptor_sets3.size(), descriptor_sets3.data(), 0, nullptr);

		model = glm::scale(MAT4_IDENTITY, VEC3_ONES * 10000.0f);
		vkCmdPushConstants(command_buffer, pipeline_skybox.get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), glm::value_ptr(model));
		material_cubemap.bind(command_buffer);
		mesh3.draw(command_buffer);


		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get_pipeline());

		std::vector<VkDescriptorSet> descriptor_sets{
			descriptor_set_frame, descriptor_set_model
		};
		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get_pipeline_layout(), 0, descriptor_sets.size(), descriptor_sets.data(), 0, nullptr);

		material.bind(command_buffer);
		mesh.draw(command_buffer);


		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_lines.get_pipeline());

		//vkCmdSetViewport(command_buffer, 0, 1, &viewport);
		//vkCmdSetScissor(command_buffer, 0, 1, &scissor);

		std::vector<VkDescriptorSet> descriptor_sets2{
			descriptor_set_frame  //, descriptor_set_model
		};

		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_lines.get_pipeline_layout(), 0, descriptor_sets2.size(), descriptor_sets2.data(), 0, nullptr);

		float angle = timestamp;
		model = glm::rotate(glm::mat4(1.0f), angle * 4.0f, VEC3_Z);

		vkCmdPushConstants(command_buffer, pipeline_lines.get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), glm::value_ptr(model));

		mesh2.draw(command_buffer);

		float angle2 = timestamp;
		model = glm::rotate(glm::mat4(1.0f), angle2 * 2.0f, VEC3_Z);

		vkCmdPushConstants(command_buffer, pipeline_lines.get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), glm::value_ptr(model));

		mesh2.draw(command_buffer);

		float angle3 = timestamp;
		model = glm::rotate(glm::mat4(1.0f), angle3 * 1.0f, VEC3_X);
		model = model * glm::scale(glm::mat4(1.0f), VEC3_ONES * 0.3f);
		vkCmdPushConstants(command_buffer, pipeline_lines.get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), glm::value_ptr(model));

		mesh2.draw(command_buffer);

		float length = std::fmodf(timestamp, 10.0f);
		timed_geometry_builder.add_cube(VEC3_ZERO, length, COLOR_RED, 0.2f);
		GeometryMesh temp_mesh;
		temp_mesh.init_resource(ctx, timed_geometry_builder.build_buffer());
		model = glm::mat4(1.0f);
		vkCmdPushConstants(command_buffer, pipeline_lines.get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), glm::value_ptr(model));
		temp_mesh.draw(command_buffer);

	}

	void on_resize(uint32_t width, uint32_t height) {
		std::cout << "resize: " << width << " " << height << "\n";
		if (!(width > 0 && height > 0)) {
			return;
		}
		auto& ctx = *ctxptr;
		ctx.resize(width, height);
	}

	bool is_key_down(uint32_t key) {
		auto it = key_states.find(key);
		if (it == key_states.end()) {
			return false;
		}
		return it->second;
	}

	bool is_mouse_down(uint32_t mouse_button) {
		return mouse_states[mouse_button];
	}

	void on_key_down(uint32_t key) {
		key_states[key] = true;
		if (key == VK_SPACE) {
			timed_geometry_builder.add_sphere(glm::vec3(0.0f, 5.0f, 0.0f), 2.0f, COLOR_MAGENTA, 3.0f);
		}
		else if (key == 'M') {
			timed_geometry_builder.add_line(VEC3_ZERO, glm::vec3(0.0f, 2.0f, 0.0f), COLOR_CYAN, 5.0f);
			timed_geometry_builder.add_cube(glm::vec3(0.0f, 5.0f, 0.0f), 4.0f, COLOR_MAGENTA, 3.0f);
		}
	}

	void on_key_up(uint32_t key) {
		key_states[key] = false;
	}

	void on_mouse_down(uint32_t mouse_button, uint32_t x, uint32_t y) {
		mouse_states[mouse_button] = true;
	}

	void on_mouse_up(uint32_t mouse_button, uint32_t x, uint32_t y) {
		mouse_states[mouse_button] = false;
	}

	void on_mouse_move(uint32_t x, uint32_t y) {
		int delta_x = (int)x - (int)mouse_move_x;
		int delta_y = (int)y - (int)mouse_move_y;
		mouse_move_x = x;
		mouse_move_y = y;
		float rotate_divide = 300.0f;
		if (is_mouse_down(MouseButtonLeft)) {
			camera_controller.rotate(-delta_x / rotate_divide, -delta_y / rotate_divide);
		}
		
	}

public:
	VulkanContext* ctxptr;
	Window* m_window;

	ResourceManager resource_manager;

	BasicPipeline pipeline;
	BasicPipeline pipeline_lines;
	BasicPipeline pipeline_skybox;

	double timestamp = 0.0;

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
	Mesh mesh;
	GeometryMesh mesh2;
	Mesh mesh3;
	Material material;
	Material material_cubemap;

	std::map<uint32_t, bool> key_states;
	bool mouse_states[3] = { 0 };
	uint32_t mouse_move_x;
	uint32_t mouse_move_y;

	Camera camera;
	CameraController camera_controller;
	glm::vec3 old_translation;

	FrameUniforms frame_uniform;
	FrameUniforms frame_uniform2;
	ModelUniforms model_uniform;

	GeometryBuilder geometry_builder;
	TimedGeometryBuilder timed_geometry_builder;
};

int main() {
	Window window;

	int width = 600;
	int height = 600;

	window.init(width, height, L"toy");

	HINSTANCE hinstance = window.get_win32_hinstance();
	HWND hwnd = window.get_win32_hwnd();

	VulkanContext vulkan_context(hinstance, hwnd, width, height);

	App app;
	std::cout << "startup\n";
	app.startup(vulkan_context, &window);

	std::cout << "mainloop\n";
	window.mainloop();

	std::cout << "shutdown\n";
	app.shutdown();
	return 0;
}
