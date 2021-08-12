#pragma once

#include <vector>
#include <memory>

#include <glm/mat4x4.hpp>
#include "mesh.h"
#include "geometry_mesh.h"
#include "material.h"


struct MeshRenderCommand {
	std::shared_ptr<Mesh> mesh;
	glm::mat4 matrix;
	std::shared_ptr<Material> material;
	AABB world_aabb;
};

struct GeometryMeshRenderCommand {
	std::shared_ptr<GeometryMesh> mesh;
	glm::mat4 matrix;
	std::shared_ptr<Material> material;
};

struct PartSlashTotal {
	std::size_t part = 0;
	std::size_t total = 0;
};

struct RenderManagerDebugInfo {
	PartSlashTotal render_culled;
	std::vector<PartSlashTotal> depth_culled_list;
};

class RenderManager {
public:
	RenderManager();
	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;

public:
	void add_mesh(std::shared_ptr<Mesh> mesh, glm::mat4 matrix, std::shared_ptr<Material> material);
	void add_geometry_mesh(std::shared_ptr<GeometryMesh> mesh, glm::mat4 matrix, std::shared_ptr<Material> material);

	void update(float delta_time);

	void render(VkCommandBuffer command_buffer, std::vector<VkDescriptorSet> descriptor_sets, glm::mat4 view_projection);
	void render_depth(VkCommandBuffer command_buffer, BasicPipeline& pipeline, glm::mat4 view_projection);

	RenderManagerDebugInfo get_debug_info();

private:
	std::vector<MeshRenderCommand> m_mesh_render_commands;
	std::vector<GeometryMeshRenderCommand> m_geometry_mesh_render_commands;

	RenderManagerDebugInfo m_debug_info;
};
