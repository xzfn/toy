#include "load_helper.h"

#include <memory>

#include "global_app.h"


std::unique_ptr<Texture> create_texture(std::string path)
{
	App& app = *get_app();
	std::string real_path = app.resource_manager.full_path(path);
	imageutil::Image raw_image = imageutil::load_image_force_channels(real_path, 4);
	auto texture = std::make_unique<Texture>();
	texture->init(app.get_ctx(), raw_image);
	return texture;
}

std::unique_ptr<TextureCubemap> create_texture_cubemap(std::vector<std::string> paths)
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

std::unique_ptr<Material> create_material()
{
	return std::unique_ptr<Material>();
}
