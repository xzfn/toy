#include "load_helper.h"

#include <memory>
#include <string>
#include <vector>

#include "color_util.h"

#include "global_app.h"


std::shared_ptr<Texture> create_texture(std::string path)
{
	App& app = *get_app();
	std::string real_path = app.resource_manager.full_path(path);
	imageutil::Image raw_image = imageutil::load_image_force_channels(real_path, 4);
	auto texture = std::make_unique<Texture>();
	texture->init(app.get_ctx(), raw_image);
	return texture;
}

std::shared_ptr<Texture> create_color_texture(glm::vec3 color) {
	// single color, 2x2, RGB image
	App& app = *get_app();
	uint32_t pixel = rgb_to_hex(color);
	imageutil::Image raw_image = imageutil::create_color_image(2, 2, 4, pixel);  // TODO change 4 to 3 when supported
	auto texture = std::make_unique<Texture>();
	texture->init(app.get_ctx(), raw_image);
	return texture;
}

std::shared_ptr<Texture> create_rgba32_texture(int width, int height, std::string data)
{
	App& app = *get_app();
	std::vector<uint8_t> bytes_data(data.begin(), data.end());
	imageutil::Image raw_image = imageutil::create_rgba32_image(width, height, bytes_data);
	auto texture = std::make_unique<Texture>();
	texture->init(app.get_ctx(), raw_image);
	return texture;
}

std::shared_ptr<TextureCubemap> create_texture_cubemap(std::vector<std::string> paths)
{
	App& app = *get_app();
	if (paths.size() != 6) {
		throw std::runtime_error("TextureCubemap need 6 images");
	}
	std::vector<imageutil::Image> raw_images;
	for (auto& path : paths) {
		std::string real_path = app.resource_manager.full_path(path);
		imageutil::Image raw_image = imageutil::load_image_force_channels(real_path, 4);
		raw_images.push_back(raw_image);
	}
	auto texture = std::make_unique<TextureCubemap>();
	texture->init(app.get_ctx(), raw_images);
	return texture;
}

std::shared_ptr<Material> create_material(std::shared_ptr<BasicPipeline> pipeline, std::shared_ptr<Texture> texture)
{
	App& app = *get_app();
	auto material = std::make_unique<Material>();
	material->init(app.get_ctx(), *pipeline.get(), *texture.get());
	return material;
}

std::shared_ptr<BasicPipeline> create_basic_pipeline(std::string vert_spv, std::string frag_spv)
{
	App& app = *get_app();
	ResourceManager& resource_manager = app.resource_manager;
	auto pipeline = std::make_unique<BasicPipeline>();
	PipelineDescription desc;
	desc.filename_vert_spv = resource_manager.full_path(vert_spv);
	desc.filename_frag_spv = resource_manager.full_path(frag_spv);
	desc.vertex_format = VertexFormat::PositionNormalUv;
	desc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	desc.cull_model_flags = VK_CULL_MODE_BACK_BIT;
	desc.render_pass = app.get_ctx().basic.render_pass;
	pipeline->init_pipeline(app.get_ctx(), desc);
	return pipeline;
}

std::shared_ptr<BasicPipeline> create_imgui_pipeline()
{
	App& app = *get_app();
	ResourceManager& resource_manager = app.resource_manager;
	auto pipeline = std::make_unique<BasicPipeline>();
	PipelineDescription desc;
	desc.filename_vert_spv = resource_manager.full_path("shader/imgui.vert.spv");
	desc.filename_frag_spv = resource_manager.full_path("shader/imgui.frag.spv");
	desc.vertex_format = VertexFormat::ImGuiPositionUvColor;
	desc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	desc.cull_model_flags = VK_CULL_MODE_NONE;
	desc.render_pass = app.get_ctx().basic.render_pass;
	pipeline->init_pipeline(app.get_ctx(), desc);
	return pipeline;
}

std::shared_ptr<Mesh> create_mesh(MeshData& mesh_data)
{
	App& app = *get_app();
	auto mesh = std::make_unique<Mesh>();
	mesh->init_resource(app.get_ctx(), mesh_data);
	return mesh;
}

std::shared_ptr<GeometryMesh> create_geometry_mesh(GeometryMeshData& mesh_data)
{
	App& app = *get_app();
	auto mesh = std::make_unique<GeometryMesh>();
	mesh->init_resource(app.get_ctx(), mesh_data);
	return mesh;
}

std::shared_ptr<ImGuiMesh> create_imgui_mesh(std::string vertex_data, std::string index_data)
{
	App& app = *get_app();
	auto mesh = std::make_unique<ImGuiMesh>();
	mesh->init_resource(app.get_ctx(), vertex_data, index_data);
	return mesh;
}
