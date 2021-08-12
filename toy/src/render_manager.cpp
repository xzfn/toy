
#include "render_manager.h"

#include <iostream>
#include <format>

#include <glm/ext.hpp>

#include "basic_pipeline.h"
#include "global_app.h"
#include "culling_util.h"


RenderManager::RenderManager()
{
}

void RenderManager::add_mesh(std::shared_ptr<Mesh> mesh, glm::mat4 matrix, std::shared_ptr<Material> material)
{
	AABB local_aabb = mesh->get_bounding_box();
	AABB world_aabb = transform_aabb(matrix, local_aabb);
	MeshRenderCommand render_command{
		mesh, matrix, material, world_aabb
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

void RenderManager::update(float delta_time)
{
	(void)delta_time;
	m_debug_info.depth_culled_list.clear();
}

void RenderManager::render(VkCommandBuffer command_buffer, std::vector<VkDescriptorSet> descriptor_sets, glm::mat4 view_projection)
{
	std::size_t total = m_mesh_render_commands.size();
	std::size_t culled = 0;

	auto frustum_planes = view_projection_planes(view_projection);
	for (auto& render_command : m_mesh_render_commands) {
		if (!planes_intersect_aabb(frustum_planes, render_command.world_aabb)) {
			++culled;
			continue;
		}
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

	m_debug_info.render_culled = PartSlashTotal(culled, total);

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

void RenderManager::render_depth(VkCommandBuffer command_buffer, BasicPipeline& pipeline, glm::mat4 view_projection) {
	std::size_t total = m_mesh_render_commands.size();
	std::size_t culled = 0;

	auto frustum_planes = view_projection_planes(view_projection);
	for (auto& render_command : m_mesh_render_commands) {
		if (!planes_intersect_aabb(frustum_planes, render_command.world_aabb)) {
			++culled;
			continue;
		}
		Mesh& mesh = *render_command.mesh;
		glm::mat4& matrix = render_command.matrix;
		pipeline.push_constants_matrix(command_buffer, matrix);
		mesh.draw(command_buffer);
	}

	m_debug_info.depth_culled_list.push_back(PartSlashTotal(culled, total));
}

RenderManagerDebugInfo RenderManager::get_debug_info()
{
	return m_debug_info;
}
