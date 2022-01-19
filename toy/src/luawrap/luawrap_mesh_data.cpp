#include "luawrap_mesh_data.h"

#include "mesh_data.h"
#include "mesh_data_helper.h"


void luawrap_VerticesData(sol::table& m) {
	auto t = m.new_usertype<VerticesData>("VerticesData");
	t[sol::meta_function::construct] = sol::constructors<VerticesData()>();

	t.set("positions", &VerticesData::positions);
	t.set("normals", &VerticesData::normals);
	t.set("uvs", &VerticesData::uvs);
}

void luawrap_MeshData(sol::table& m) {
	auto t = m.new_usertype<MeshData>("MeshData");
	t[sol::meta_function::construct] = sol::constructors<MeshData()>();

	t.set_function("set_vertices_data", &MeshData::set_vertices_data);
	t.set_function("get_vertices_data", &MeshData::get_vertices_data);
	t.set_function("set_indices", &MeshData::set_indices);
	t.set_function("get_indices", &MeshData::get_indices);

	t.set_function("create_unit_cube", sol::factories(&create_mesh_data_unit_cube));
	t.set_function("create_unit_quad", sol::factories(&create_mesh_data_unit_quad));
}

void luawrap_mesh_data(sol::table& m) {
	luawrap_VerticesData(m);
	luawrap_MeshData(m);
}
