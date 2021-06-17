#pragma once

#include <string>
#include <memory>

#include "texture.h"
#include "texture_cubemap.h"
#include "material.h"


std::unique_ptr<Texture> create_texture(std::string path);
std::unique_ptr<TextureCubemap> create_texture_cubemap(std::vector<std::string> paths);
std::unique_ptr<Material> create_material();
