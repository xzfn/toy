#include "imgui_manager.h"
#include "global_app.h"

ImGuiManager::ImGuiManager()
{
}

void ImGuiManager::add_imgui_mesh_part(
	std::shared_ptr<ImGuiMesh> mesh, glm::mat4 matrix, std::shared_ptr<Material> material,
	std::size_t element_count, std::size_t index_buffer_offset, glm::vec4 clip_rect
)
{
	ImGuiMeshRenderCommand render_command{
		mesh, matrix, material, element_count, index_buffer_offset, clip_rect
	};
	m_imgui_mesh_render_commands.push_back(render_command);
}

void ImGuiManager::render(VkCommandBuffer command_buffer, std::vector<VkDescriptorSet> descriptor_sets)
{
	for (auto& render_command : m_imgui_mesh_render_commands) {
		ImGuiMesh& mesh = *render_command.mesh;
		glm::mat4& matrix = render_command.matrix;
		Material& material = *render_command.material;
		std::size_t element_count = render_command.element_count;
		std::size_t index_buffer_offset = render_command.index_buffer_offset;
		glm::vec4 clip_rect = render_command.clip_rect;

		BasicPipeline& pipeline = *material.get_pipeline();
		pipeline.bind(command_buffer);
		pipeline.bind_descriptor_sets(command_buffer, descriptor_sets);

		material.bind(command_buffer);
		pipeline.push_constants_matrix(command_buffer, matrix);
		mesh.bind(command_buffer);

		VkRect2D scissor = {
			{int32_t(clip_rect.x), int32_t(clip_rect.y)},
			{uint32_t(clip_rect.z - clip_rect.x), uint32_t(clip_rect.w - clip_rect.y)}
		};
		vkCmdSetScissor(command_buffer, 0, 1, &scissor);
		vkCmdDrawIndexed(command_buffer, (uint32_t)element_count, 1, (uint32_t)index_buffer_offset, 0, 0);
	}

	get_app()->get_ctx().reset_scissor(command_buffer);

	m_imgui_mesh_render_commands.clear();
}
