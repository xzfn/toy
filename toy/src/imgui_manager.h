#pragma once

#include "material.h"
#include "imgui_mesh.h"


struct ImGuiMeshRenderCommand {
	std::shared_ptr<ImGuiMesh> mesh;
	glm::mat4 matrix;
	std::shared_ptr<Material> material;
	std::size_t element_count;
	std::size_t index_buffer_offset;
};


class ImGuiManager {
public:
	ImGuiManager();
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;

public:

	void add_imgui_mesh_part(std::shared_ptr<ImGuiMesh> mesh, glm::mat4 matrix, std::shared_ptr<Material> material, std::size_t element_count, std::size_t index_buffer_offset);
	void render(VkCommandBuffer command_buffer, std::vector<VkDescriptorSet> descriptor_sets);

private:
	std::vector<ImGuiMeshRenderCommand> m_imgui_mesh_render_commands;
};
