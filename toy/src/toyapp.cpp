#include "toyapp.h"

#include <pybind11/embed.h>

#include <pybind11/functional.h>
namespace py = pybind11;

#include <chrono>
#include <cmath>
#include <iostream>
#include <map>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include "global_app.h"

#include "wrap/wrap_vmath.h"
#include "wrap/wrap_toy.h"

#include "python_util.h"

#include "vulkan_helper.h"



PYBIND11_EMBEDDED_MODULE(toy, m) {
	m.attr("__author__") = "wangyueheng";

	wrap_toy(m);
	m.attr("app") = g_app;
	m.def("get_app", []() {
		return g_app;
	}, py::return_value_policy::reference);
}

PYBIND11_EMBEDDED_MODULE(vmath, m) {
	wrap_vmath(m);
}


App::App() {

}

void App::startup(VulkanContext& ctx, Window* window) {
	ctxptr = &ctx;
	m_window = window;

	resource_manager.add_resource_directory("../../toy");
	resource_manager.add_resource_directory("../../toy_generated");

	shadow_manager.init(ctx);

	py::module sys = py::module::import("sys");
	py::list sys_path = sys.attr("path");
	sys_path.append("../../toy/script");

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

	window->set_mouse_wheel_callback([this](float wheel_delta, uint32_t x, uint32_t y) {
		this->on_mouse_wheel(wheel_delta, x, y);
	});

	ShapeData shape_data;
	MeshData mesh_data;
	shape_data = generate_cube();
	mesh_data.set_vertices_data(shape_data.positions, shape_data.normals, shape_data.uvs);
	mesh_data.set_indices(shape_data.indices);
	mesh_cube.init_resource(ctx, mesh_data);

	PipelineDescription desc;
	desc.filename_vert_spv = resource_manager.full_path("shader/basic.vert.spv");
	desc.filename_frag_spv = resource_manager.full_path("shader/basic.frag.spv");
	desc.vertex_format = VertexFormat::PositionNormalUv;
	desc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	desc.cull_model_flags = VK_CULL_MODE_BACK_BIT;
	desc.render_pass = ctx.basic.render_pass;
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

	desc.filename_vert_spv = resource_manager.full_path("shader/text.vert.spv");
	desc.filename_frag_spv = resource_manager.full_path("shader/text.frag.spv");
	desc.vertex_format = VertexFormat::PositionColorUv;
	desc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	desc.cull_model_flags = VK_CULL_MODE_BACK_BIT;
	pipeline_text.init_pipeline(ctx, desc);

	// shadow
	PipelineDescription depth_desc;
	depth_desc.filename_vert_spv = resource_manager.full_path("shader/depth.vert.spv");
	depth_desc.filename_frag_spv = resource_manager.full_path("shader/depth.frag.spv");
	depth_desc.vertex_format = VertexFormat::PositionNormalUv;
	depth_desc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	depth_desc.cull_model_flags = VK_CULL_MODE_BACK_BIT;
	depth_desc.render_pass = ctx.basic.depth_render_pass;
	pipeline_depth.init_pipeline(ctx, depth_desc);


	imageutil::Image raw_image = imageutil::load_image_force_channels(resource_manager.full_path("resource/cube.png"), 4);
	texture.init(ctx, raw_image);

	material_lines = std::make_shared<Material>();
	material_lines->init(ctx, pipeline_lines, texture);

	imageutil::Image raw_image_ascii = imageutil::load_image_force_channels(resource_manager.full_path("resource/ascii_white.png"), 4);
	texture_ascii.init(ctx, raw_image_ascii);

	material_text.init(ctx, pipeline_text, texture_ascii);


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

	descriptor_set_light = ctx.create_descriptor_set(pipeline_descriptor_set_layouts.light);
	auto light_uniform_buffer_and_memory = ctx.create_uniform_buffer_coherent(
		nullptr, sizeof(LightUniforms));
	light_uniform_buffer = light_uniform_buffer_and_memory.first;
	light_uniform_memory = light_uniform_buffer_and_memory.second;
	pipeline.update_descriptor_set_light(descriptor_set_light, light_uniform_buffer, sizeof(LightUniforms));

	descriptor_set_shadow = ctx.create_descriptor_set(pipeline_descriptor_set_layouts.shadow);
	auto shadow_uniform_buffer_and_memory = ctx.create_uniform_buffer_coherent(nullptr, sizeof(ShadowUniforms));
	shadow_uniform_buffer = shadow_uniform_buffer_and_memory.first;
	shadow_uniform_memory = shadow_uniform_buffer_and_memory.second;

	// shadow
	VkSampler depth_sampler = shadow_manager.get_depth_sampler();
	VkImageView depth_image_view_array = shadow_manager.get_depth_image_view_array();
	pipeline.update_descriptor_set_shadow(descriptor_set_shadow, shadow_uniform_buffer, sizeof(ShadowUniforms), depth_sampler, depth_image_view_array);

	auto light1 = std::make_shared<Light>();
	light1->set_type(LightType::Point);
	light1->set_position(glm::vec3(5.0f, 5.0f, 5.0f));
	light1->set_color(glm::vec3(1.0f, 1.0f, 0.5f));
	light_manager.add_light(light1);
	
	auto light2 = std::make_shared<Light>();
	light2->set_type(LightType::Point);
	light2->set_position(glm::vec3(5.0f, 5.0f, -5.0f));
	light2->set_color(glm::vec3(1.0f, 0.0f, 0.5f));
	light_manager.add_light(light2);


	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
	timestamp = std::chrono::duration<double>(now).count();

	PYUTIL_CHECK_RUN_BEGIN
	auto toyentry = py::module::import("toyentry");
	toyentry.attr("startup")();
	PYUTIL_CHECK_RUN_END
}

void App::shutdown() {
	PYUTIL_SAFE_RUN_BEGIN
	auto toyentry = py::module::import("toyentry");
	toyentry.attr("shutdown")();
	PYUTIL_SAFE_RUN_END

	VulkanContext& ctx = *ctxptr;
	ctx.device_wait_idle();

	ctx.destroy_buffer_and_memory(frame_uniform_buffer, frame_uniform_memory);
	ctx.destroy_buffer_and_memory(light_uniform_buffer, light_uniform_memory);
	ctx.destroy_buffer_and_memory(shadow_uniform_buffer, shadow_uniform_memory);

	shadow_manager.destroy();

}

void App::update() {
	VulkanContext& ctx = *ctxptr;
	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
	double new_timestamp = std::chrono::duration<double>(now).count();
	delta_time = (float)(new_timestamp - timestamp);
	timestamp = new_timestamp;

	timer_manager.schedule(new_timestamp);

	try {
		auto toyentry = py::module::import("toyentry");
		toyentry.attr("update")();
	}
	catch (py::error_already_set& e) {
		if (e.matches(PyExc_SystemExit)) {
			std::cout << "python got SystemExit\n";
			m_window->set_should_close();
		}
		else {
			std::cout << "ERROR python update throws: " << e.what() << "\n";
			pyutil::call_sys_excepthook(e);
		}
	}

	VkClearColorValue clear_color;
	clear_color.float32[0] = background_color.x;
	clear_color.float32[1] = background_color.y;
	clear_color.float32[2] = background_color.z;
	clear_color.float32[3] = 1.0f;

	camera_manager.update(delta_time);

	auto extent = ctx.basic.window_extent;
	
	camera_manager.get_camera()->set_view_size((float)extent.width, (float)extent.height);

	glm::mat4 camera_view_projection = camera_manager.get_camera()->get_view_projection();
	frame_uniform.view_projection = camera_view_projection;
	frame_uniform.camera_position = camera_manager.get_camera_controller()->get_transform().translation;
	frame_uniform.sun_light_direction = light_manager.get_sun()->get_direction();
	frame_uniform.screen_width = (float)ctx.basic.extent.width;
	frame_uniform.screen_height = (float)ctx.basic.extent.height;

	void* memory_pointer;
	int frame_uniform_data_size = sizeof(frame_uniform);
	void* frame_uniform_data = &frame_uniform;
	vkMapMemory(ctx.basic.device, frame_uniform_memory, 0,
		frame_uniform_data_size, 0, &memory_pointer);
	memcpy(memory_pointer, frame_uniform_data, frame_uniform_data_size);
	vkUnmapMemory(ctx.basic.device, frame_uniform_memory);


	LightUniforms light_uniform = light_manager.build_light_uniform();

	int light_uniform_data_size = sizeof(light_uniform);
	void* light_uniform_data = &light_uniform;
	vkMapMemory(ctx.basic.device, light_uniform_memory, 0,
		light_uniform_data_size, 0, &memory_pointer);
	memcpy(memory_pointer, light_uniform_data, light_uniform_data_size);
	vkUnmapMemory(ctx.basic.device, light_uniform_memory);

	// shadow
	ShadowUniforms shadow_uniform = shadow_manager.build_shadow_uniform();

	int shadow_uniform_data_size = sizeof(shadow_uniform);
	void* shadow_uniform_data = &shadow_uniform;
	vkMapMemory(ctx.basic.device, shadow_uniform_memory, 0,
		shadow_uniform_data_size, 0, &memory_pointer);
	memcpy(memory_pointer, shadow_uniform_data, shadow_uniform_data_size);
	vkUnmapMemory(ctx.basic.device, shadow_uniform_memory);


	ctx.render(clear_color,
		[this](VkCommandBuffer command_buffer) {
		this->depth_pass(command_buffer);},
		[this](VkCommandBuffer command_buffer) { this->render(command_buffer); }
	);

	timed_geometry_builder.update(delta_time);
	timed_text_builder.update(delta_time);

	input_manager.clear_frame();
}

void App::depth_pass(VkCommandBuffer command_buffer)
{
	shadow_manager.render_depth_pass(command_buffer, pipeline_depth, light_manager, render_manager);
}

void App::render(VkCommandBuffer command_buffer) {
	//return;
	VulkanContext& ctx = *ctxptr;

	glm::mat4 model(1.0f);

	std::vector<VkDescriptorSet> descriptor_sets_frame{
		descriptor_set_frame,
		descriptor_set_light,
		descriptor_set_shadow
	};


	// skybox
	if (camera_manager.get_camera()->is_perspective()) {
		model = glm::scale(MAT4_IDENTITY, VEC3_ONES * 10000.0f);
		material_cubemap.get_pipeline()->bind(command_buffer);
		material_cubemap.get_pipeline()->bind_descriptor_sets(command_buffer, descriptor_sets_frame);
		material_cubemap.bind(command_buffer);
		material_cubemap.get_pipeline()->push_constants_matrix(command_buffer, model);
		mesh_cube.draw(command_buffer);
	}

	// global timed lines
	material_lines->get_pipeline()->bind(command_buffer);
	material_lines->get_pipeline()->bind_descriptor_sets(command_buffer, descriptor_sets_frame);

	GeometryMesh transient_geometry_mesh;
	transient_geometry_mesh.init_resource(ctx, timed_geometry_builder.build_buffer());
	model = glm::mat4(1.0f);
	material_lines->get_pipeline()->push_constants_matrix(command_buffer, model);
	transient_geometry_mesh.draw(command_buffer);

	// render_manager
	render_manager.render(command_buffer, descriptor_sets_frame);

	// global timed text
	material_text.get_pipeline()->bind(command_buffer);
	material_text.get_pipeline()->bind_descriptor_sets(command_buffer, descriptor_sets_frame);

	VertexMesh transient_mesh_text;
	transient_mesh_text.init_resource(ctx, timed_text_builder.build_buffer());
	model = glm::mat4(1.0f);
	material_text.bind(command_buffer);
	material_text.get_pipeline()->push_constants_matrix(command_buffer, model);
	transient_mesh_text.draw(command_buffer);

}

void App::on_resize(uint32_t width, uint32_t height) {
	std::cout << "resize: " << width << " " << height << "\n";
	if (!(width > 0 && height > 0)) {
		return;
	}
	auto& ctx = *ctxptr;
	ctx.resize(width, height);
}


void App::on_key_down(uint32_t key) {
	input_manager.input_key_down(key);
	if (script_on_key_down) {
		PYUTIL_SAFE_RUN_BEGIN
		script_on_key_down(key);
		PYUTIL_SAFE_RUN_END
	}

	if (key == VK_NUMPAD1) {
		// test reload shader
		pipeline_text.reload_shader();
	}
}

void App::on_key_up(uint32_t key) {
	input_manager.input_key_up(key);
}

void App::on_mouse_down(uint32_t mouse_button, uint32_t x, uint32_t y) {
	(void)x;
	(void)y;
	input_manager.input_mouse_button_down(mouse_button);
}

void App::on_mouse_up(uint32_t mouse_button, uint32_t x, uint32_t y) {
	(void)x;
	(void)y;
	input_manager.input_mouse_button_up(mouse_button);
}

void App::on_mouse_move(uint32_t x, uint32_t y) {
	input_manager.input_mouse_move(x, y);
}

void App::on_mouse_wheel(float wheel_delta, uint32_t x, uint32_t y) {
	(void)x;
	(void)y;
	input_manager.input_mouse_wheel(wheel_delta);
}

void App::set_background_color(glm::vec3 color)
{
	background_color = color;
}

glm::vec2 App::get_view_size()
{
	VkExtent2D extent = ctxptr->basic.extent;
	return glm::vec2{ extent.width, extent.height };
}

VulkanContext& App::get_ctx()
{
	return *ctxptr;
}
