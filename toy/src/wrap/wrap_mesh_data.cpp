#include "wrap_mesh_data.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "mesh_data.h"
#include "mesh_data_helper.h"


void wrap_VerticesData(py::module_& m) {
	py::class_<VerticesData> t(m, "VerticesData");
	t.def(py::init<>());
	t.def_readwrite("positions", &VerticesData::positions);
	t.def_readwrite("normals", &VerticesData::normals);
	t.def_readwrite("uvs", &VerticesData::uvs);
}

void wrap_MeshData(py::module_& m) {
	py::class_<MeshData> t(m, "MeshData");
	t.def(py::init<>());
	t.def("set_vertices_data", &MeshData::set_vertices_data);
	t.def("get_vertices_data", &MeshData::get_vertices_data);
	t.def("set_indices", &MeshData::set_indices);
	t.def("get_indices", &MeshData::get_indices);

	t.def_static("create_unit_cube", &create_mesh_data_unit_cube);
	t.def_static("create_unit_quad", &create_mesh_data_unit_quad);
}

void wrap_mesh_data(pybind11::module_& m) {
	wrap_VerticesData(m);
	wrap_MeshData(m);
}
