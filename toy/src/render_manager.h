#pragma once

#include <vector>
#include <memory>

#include <glm/mat4x4.hpp>
#include "mesh.h"
#include "material.h"


struct MeshRenderCommand {
	std::shared_ptr<Mesh> mesh;
	glm::mat4 matrix;
	std::shared_ptr<Material> material;
};


class RenderManager {
public:
	RenderManager();
	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;

public:
	void add_mesh(std::shared_ptr<Mesh> mesh, glm::mat4 matrix, std::shared_ptr<Material> material);

	void render(VkCommandBuffer command_buffer, std::vector<VkDescriptorSet> descriptor_sets);

private:
	std::vector<MeshRenderCommand> m_mesh_render_commands;
};
