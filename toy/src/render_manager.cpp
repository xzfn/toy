
#include "render_manager.h"

#include <glm/ext.hpp>

#include "basic_pipeline.h"
#include "global_app.h"


RenderManager::RenderManager()
{
}

void RenderManager::add_mesh(std::shared_ptr<Mesh> mesh, glm::mat4 matrix, std::shared_ptr<Material> material)
{
	MeshRenderCommand render_command{
		mesh, matrix, material
	};
	m_mesh_render_commands.push_back(render_command);
}

void RenderManager::add_geometry_mesh(std::shared_ptr<GeometryMesh> mesh, glm::mat4 matrix, std::shared_ptr<Material> material)
{
	GeometryMeshRenderCommand render_command{
		mesh, matrix, material
	};
	m_geometry_mesh_render_commands.push_back(render_command);
}

void RenderManager::render(VkCommandBuffer command_buffer, std::vector<VkDescriptorSet> descriptor_sets)
{

	for (auto& render_command : m_mesh_render_commands) {
		Mesh& mesh = *render_command.mesh;
		glm::mat4& matrix = render_command.matrix;
		Material& material = *render_command.material;

		BasicPipeline& pipeline = *material.get_pipeline();
		pipeline.bind(command_buffer);
		pipeline.bind_descriptor_sets(command_buffer, descriptor_sets);

		material.bind(command_buffer);
		pipeline.push_constants_matrix(command_buffer, matrix);
		mesh.draw(command_buffer);
	}

	for (auto& render_command : m_geometry_mesh_render_commands) {
		GeometryMesh& mesh = *render_command.mesh;
		glm::mat4& matrix = render_command.matrix;
		Material& material = *render_command.material;

		BasicPipeline& pipeline = *material.get_pipeline();
		pipeline.bind(command_buffer);
		pipeline.bind_descriptor_sets(command_buffer, descriptor_sets);

		material.bind(command_buffer);
		pipeline.push_constants_matrix(command_buffer, matrix);
		mesh.draw(command_buffer);
	}

	m_mesh_render_commands.clear();
	m_geometry_mesh_render_commands.clear();
}

void RenderManager::render_depth(VkCommandBuffer command_buffer, BasicPipeline& pipeline) {
	for (auto& render_command : m_mesh_render_commands) {
		Mesh& mesh = *render_command.mesh;
		glm::mat4& matrix = render_command.matrix;
		pipeline.push_constants_matrix(command_buffer, matrix);
		mesh.draw(command_buffer);
	}
}
