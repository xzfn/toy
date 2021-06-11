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


struct VertexData {
	float x, y, z, w;
	float u, v;
};


glm::vec3 tuple_to_vec3(py::tuple t) {
	return glm::vec3(t[0].cast<float>(), t[1].cast<float>(), t[2].cast<float>());
}

py::tuple vec3_to_tuple(glm::vec3 v) {
	py::tuple t(3);
	t[0] = v.x;
	t[1] = v.y;
	t[2] = v.z;
	return t;
}

glm::quat tuple_to_quat(py::tuple t) {
	return glm::quat(t[0].cast<float>(), t[1].cast<float>(), t[2].cast<float>(), t[3].cast<float>());
}

py::tuple quat_to_tuple(glm::quat q) {
	py::tuple t(4);
	t[0] = q.w;
	t[1] = q.x;
	t[2] = q.y;
	t[3] = q.z;
	return t;
}

namespace pybind11 {
namespace detail {

template <> struct type_caster<glm::vec3> : public type_caster_base<glm::vec3> {
	using base = type_caster_base<glm::vec3>;
public:
	bool load(handle src, bool convert) {
		if (base::load(src, convert)) {
			return true;
		}
		if (py::isinstance<py::tuple>(src)) {
			glm::vec3 v = tuple_to_vec3(src.cast<py::tuple>());
			value = new glm::vec3(v.x, v.y, v.z);
			return true;
		}
		return false;
	}

	static handle cast(glm::vec3 src, return_value_policy policy, handle parent) {
		// if g_app->custom_vec3_builder:
		//     return g_app->custom_vec3_builder(src.x, src.y, src.z).release();
		return base::cast(src, policy, parent);
		// return vec3_to_tuple(src).release();
	}
};

template <> struct type_caster<glm::quat> {
public:
	PYBIND11_TYPE_CASTER(glm::quat, _("glm::quat"));
	bool load(handle src, bool convert) {
		// from pybind11/cast.h load_impl
		PyTypeObject* srctype = Py_TYPE(src.ptr());
		type_info* typeinfo = get_type_info(std::type_index(typeid(glm::quat)));
		if (srctype == typeinfo->type) {
			auto& v_h = reinterpret_cast<instance*>(src.ptr())->get_value_and_holder();
			assert(v_h.holder_constructed());
			value = *(v_h.value_ptr<glm::quat>());
			return true;
		}

		if (py::isinstance<py::tuple>(src)) {
			value = tuple_to_quat(src.cast<py::tuple>());
			return true;
		}
		return false;
	}
	static handle cast(glm::quat src, return_value_policy policy, handle parent) {
		auto inst = reinterpret_steal<object>(make_new_instance(get_type_info(std::type_index(typeid(glm::quat)))->type));
		auto wrapper = reinterpret_cast<instance*>(inst.ptr());
		auto& v_h = wrapper->get_value_and_holder();
		wrapper->owned = true;
		void*& p = values_and_holders(wrapper).begin()->value_ptr();
		p = new glm::quat(src.w, src.x, src.y, src.z);
		return inst.release();
	}
};

}
}



PYBIND11_EMBEDDED_MODULE(toy, m) {
	m.attr("__author__") = "wangyueheng";


	m.def("draw_line", [](glm::vec3 start, glm::vec3 end, glm::vec3 color, float duration) {
		g_app->timed_geometry_builder.add_line(start, end, color, duration);
	});

	m.def("draw_text", [](glm::vec3 position, std::string text, glm::vec3 color, float duration) {
		g_app->timed_text_builder.add_text(position, text, color, duration);
	});

	m.def("draw_screen_text", [](glm::vec3 position, std::string text, glm::vec3 color, float duration) {
		g_app->timed_text_builder.add_screen_text(position, text, color, duration);
	});

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

	float axis_length = 5.0f;
	geometry_builder.add_line(VEC3_ZERO, VEC3_X * axis_length, COLOR_RED);
	geometry_builder.add_line(VEC3_ZERO, VEC3_Y * axis_length, COLOR_GREEN);
	geometry_builder.add_line(VEC3_ZERO, VEC3_Z * axis_length, COLOR_BLUE);
	geometry_builder.add_cube(VEC3_X * axis_length, 1.0f, COLOR_RED);
	geometry_builder.add_cube(VEC3_Y * axis_length, 1.0f, COLOR_GREEN);
	geometry_builder.add_cube(VEC3_Z * axis_length, 1.0f, COLOR_BLUE);
	geometry_builder.add_sphere(VEC3_X * 4.0f, 2.0f, COLOR_GRAY);
	geometry_builder.add_sphere(VEC3_X * 10.0f, 2.0f, COLOR_GRAY);

	mesh2.init_resource(ctx, geometry_builder.build_buffer());


	text_builder.add_screen_text(VEC3_ZERO + glm::vec3(0.0f, 20.0f, 0.0f), "ABCDEFG!@#$%^&abcdefg", COLOR_BLUE);

	text_builder.add_screen_text(glm::vec3(0.0f, 300.0f, 0.0f), "(_+|)", COLOR_YELLOW);

	text_builder.add_screen_text(glm::vec3(300.0f, 300.0f, 0.0f), "(_+|)", COLOR_YELLOW);

	//text_builder.add_text(glm::vec3(0.0f, 5.0f, 0.0f), "this is test text2", COLOR_BLUE);
	mesh_text.init_resource(ctx, text_builder.build_buffer());

	ShapeData shape_data = generate_unit_cube();
	MeshData mesh_data;
	mesh_data.set_vertices_data(shape_data.positions, shape_data.normals, shape_data.uvs);
	mesh_data.set_indices(shape_data.indices);
	mesh.init_resource(ctx, mesh_data);

	shape_data = generate_cube();
	mesh_data.set_vertices_data(shape_data.positions, shape_data.normals, shape_data.uvs);
	mesh_data.set_indices(shape_data.indices);
	mesh3.init_resource(ctx, mesh_data);

	p_mesh = std::make_shared<Mesh>();
	p_mesh->init_resource(ctx, mesh_data);


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

	desc.filename_vert_spv = resource_manager.full_path("shader/text.vert.spv");
	desc.filename_frag_spv = resource_manager.full_path("shader/text.frag.spv");
	desc.vertex_format = VertexFormat::PositionColorUv;
	desc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	desc.cull_model_flags = VK_CULL_MODE_BACK_BIT;
	pipeline_text.init_pipeline(ctx, desc);


	imageutil::Image raw_image = imageutil::load_image_force_channels(resource_manager.full_path("resource/cube.png"), 4);
	texture.init(ctx, raw_image);

	material.init(ctx, pipeline, texture);

	p_material = std::make_shared<Material>();
	p_material->init(ctx, pipeline, texture);

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

	descriptor_set_frame2 = ctx.create_descriptor_set(pipeline_descriptor_set_layouts.model);
	frame_uniform_buffer_and_memory = ctx.create_uniform_buffer_coherent(
		nullptr, sizeof(FrameUniforms));
	frame_uniform_buffer2 = frame_uniform_buffer_and_memory.first;
	frame_uniform_memory2 = frame_uniform_buffer_and_memory.second;
	pipeline.update_descriptor_set_frame(descriptor_set_frame2, frame_uniform_buffer2, sizeof(FrameUniforms));


	descriptor_set_model = ctx.create_descriptor_set(pipeline_descriptor_set_layouts.model);
	auto model_uniform_buffer_and_memory = ctx.create_uniform_buffer_coherent(
		nullptr, sizeof(ModelUniforms));
	model_uniform_buffer = model_uniform_buffer_and_memory.first;
	model_uniform_memory = model_uniform_buffer_and_memory.second;
	pipeline.update_descriptor_set_model(descriptor_set_model, model_uniform_buffer, sizeof(ModelUniforms));


	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
	timestamp = std::chrono::duration<double>(now).count();

	try {
		auto& toyentry = py::module::import("toyentry");
		toyentry.attr("startup")();
	}
	catch (py::error_already_set& e) {
		std::cout << "py init failed: " << e.what() << "\n";
		assert(false);
	}
}

void App::shutdown() {
	try {
		auto& toyentry = py::module::import("toyentry");
		toyentry.attr("shutdown")();
	}
	catch (py::error_already_set& e) {
		std::cout << "py shutdown failed: " << e.what() << "\n";
	}

	VulkanContext& ctx = *ctxptr;
	ctx.device_wait_idle();

	ctx.destroy_buffer_and_memory(model_uniform_buffer, model_uniform_memory);
	ctx.destroy_buffer_and_memory(frame_uniform_buffer, frame_uniform_memory);
	ctx.destroy_buffer_and_memory(frame_uniform_buffer2, frame_uniform_memory2);

}

void App::update() {
	VulkanContext& ctx = *ctxptr;
	auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
	double new_timestamp = std::chrono::duration<double>(now).count();
	delta_time = new_timestamp - timestamp;
	timestamp = new_timestamp;

	timer_manager.schedule(new_timestamp);

	try {
		auto& toyentry = py::module::import("toyentry");
		toyentry.attr("update")();
	}
	catch (py::error_already_set& e) {
		if (e.matches(PyExc_SystemExit)) {
			std::cout << "py update got SystemExit\n";
			m_window->set_should_close();
		}
		else {
			std::cout << "py update failed: " << e.what() << "\n";
		}
	}



	double integral;
	float fractional = std::modf(timestamp / 10.0f, &integral);
	VkClearColorValue clear_color;
	clear_color.float32[0] = background_color.x;
	clear_color.float32[1] = background_color.y;
	clear_color.float32[2] = background_color.z;
	clear_color.float32[3] = 1.0f;

	float theta = timestamp;
	float cos_theta = cosf(theta);
	float sin_theta = sinf(theta);

	camera_manager.update(delta_time);

	auto extent = ctx.basic.window_extent;
	
	camera_manager.get_camera()->set_view_size(extent.width, extent.height);


	auto& p_material_uniform = p_material->ref_uniforms();
	p_material_uniform.base_color = glm::vec3(cos_theta, 1.0f, sin_theta);
	for (int i = 0; i < 10; ++i) {
		Transform transform;
		transform.translation = glm::vec3(0.0f, 5 + i * 4.0f, 0.0f);
		glm::mat4 matrix = transform_to_mat4(transform);
		render_manager.add_mesh(p_mesh, matrix, p_material);
	}


	glm::mat4 model = glm::rotate(glm::mat4(1.0), theta / 5.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 model2 = glm::rotate(glm::mat4(1.0), theta, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 camera_view_projection = camera_manager.get_camera()->get_view_projection();
	frame_uniform.view_projection = camera_view_projection;
	theta = timestamp * 3.0f;
	cos_theta = cosf(theta);
	sin_theta = sinf(theta);
	frame_uniform.sun_light_direction = glm::vec3(cos_theta, 0.5f, sin_theta);
	frame_uniform.screen_width = ctx.basic.extent.width;
	frame_uniform.screen_height = ctx.basic.extent.height;

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
	timed_text_builder.update(delta_time);

	input_manager.clear_frame();
}

void App::render(VkCommandBuffer command_buffer) {
	VulkanContext& ctx = *ctxptr;

	glm::mat4 model;

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_skybox.get_pipeline());
	std::vector<VkDescriptorSet> descriptor_sets3{
		descriptor_set_frame  //, descriptor_set_model
	};
	vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_lines.get_pipeline_layout(), 0, descriptor_sets3.size(), descriptor_sets3.data(), 0, nullptr);

	if (camera_manager.get_camera()->is_perspective()) {
		model = glm::scale(MAT4_IDENTITY, VEC3_ONES * 10000.0f);
		vkCmdPushConstants(command_buffer, pipeline_skybox.get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), glm::value_ptr(model));
		material_cubemap.bind(command_buffer);
		mesh3.draw(command_buffer);
	}



	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get_pipeline());

	std::vector<VkDescriptorSet> descriptor_sets{
		descriptor_set_frame
	};
	vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get_pipeline_layout(), 0, descriptor_sets.size(), descriptor_sets.data(), 0, nullptr);
	model = glm::mat4(1.0f);
	vkCmdPushConstants(command_buffer, pipeline_lines.get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), glm::value_ptr(model));

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



	render_manager.render(command_buffer);


	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_text.get_pipeline());
	std::vector<VkDescriptorSet> descriptor_sets4{
		descriptor_set_frame  //, descriptor_set_model
	};
	vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_lines.get_pipeline_layout(), 0, descriptor_sets3.size(), descriptor_sets3.data(), 0, nullptr);


	// draw text last
	model = glm::mat4(1.0f) * 0.1f;
	vkCmdPushConstants(command_buffer, pipeline_text.get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), glm::value_ptr(model));
	material_text.bind(command_buffer);
	mesh_text.draw(command_buffer);
	
	vkCmdPushConstants(command_buffer, pipeline_text.get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), glm::value_ptr(model));
	material_text.bind(command_buffer);
	VertexMesh mesh_text_temp;
	TextBuilder text_builder_temp;
	text_builder_temp.add_text(glm::vec3(0.0f, 10.0f, 0.0f), std::to_string(timestamp));
	mesh_text_temp.init_resource(ctx, text_builder_temp.build_buffer());
	mesh_text_temp.draw(command_buffer);

	VertexMesh mesh_text_temp2;
	timed_text_builder.add_text(glm::vec3(0.0f, 0.0f, 0.0f), "Origin", COLOR_CYAN, 0.0f);
	timed_text_builder.add_text(VEC3_X * 10.0f, "X", COLOR_CYAN, 0.0f);
	timed_text_builder.add_text(VEC3_Y * 10.0f, "Y", COLOR_CYAN, 0.0f);
	timed_text_builder.add_text(VEC3_Z * 10.0f, "Z", COLOR_CYAN, 0.0f);

	mesh_text_temp2.init_resource(ctx, timed_text_builder.build_buffer());
	model = glm::mat4(1.0f);
	vkCmdPushConstants(command_buffer, pipeline_text.get_pipeline_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model), glm::value_ptr(model));
	mesh_text_temp2.draw(command_buffer);


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
		try {
			script_on_key_down(key);
		}
		catch (std::exception& e) {
			std::cout << "script_on_key_down error" << e.what() << "\n";
		}
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
	input_manager.input_mouse_button_down(mouse_button);
}

void App::on_mouse_up(uint32_t mouse_button, uint32_t x, uint32_t y) {
	input_manager.input_mouse_button_up(mouse_button);
}

void App::on_mouse_move(uint32_t x, uint32_t y) {
	input_manager.input_mouse_move(x, y);
}

void App::on_mouse_wheel(float wheel_delta, uint32_t x, uint32_t y) {
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
