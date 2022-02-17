#pragma once

#include <string>
#include <memory>

#include "texture.h"
#include "texture_cubemap.h"
#include "material.h"
#include "basic_pipeline.h"
#include "mesh.h"
#include "geometry_mesh.h"
#include "imgui_mesh.h"


std::shared_ptr<Texture> create_texture(std::string path);
std::shared_ptr<Texture> create_color_texture(glm::vec3 color);
std::shared_ptr<Texture> create_rgba32_texture(int width, int height, std::string data);
std::shared_ptr<TextureCubemap> create_texture_cubemap(std::vector<std::string> paths);
std::shared_ptr<Material> create_material(std::shared_ptr<BasicPipeline> pipeline, std::shared_ptr<Texture> texture);
std::shared_ptr<BasicPipeline> create_basic_pipeline(std::string vert_spv, std::string frag_spv);
std::shared_ptr<BasicPipeline> create_imgui_pipeline();
std::shared_ptr<Mesh> create_mesh(MeshData& mesh_data);
std::shared_ptr<GeometryMesh> create_geometry_mesh(GeometryMeshData& mesh_data);
std::shared_ptr<ImGuiMesh> create_imgui_mesh(std::string vertex_data, std::string index_data);
