#include "wrap_geometry_builder.h"

#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;

#include "geometry_builder.h"


void wrap_GeometryMeshData(py::module_& m) {
	py::class_<GeometryMeshData> t(m, "GeometryMeshData");
	t.def(py::init<>());
}

void wrap_GeometryBuilder(py::module_& m) {
	py::class_<GeometryBuilder> t(m, "GeometryBuilder");
	t.def(py::init<>());
	t.def("add_line", &GeometryBuilder::add_line);
	t.def("add_sphere", &GeometryBuilder::add_sphere);
	t.def("add_cube", &GeometryBuilder::add_cube);
	t.def("build_data", &GeometryBuilder::build_data);
}

void wrap_TimedGeometryBuilder(py::module_& m) {
	py::class_<TimedGeometryBuilder> t(m, "TimedGeometryBuilder");
	t.def(py::init<>());
	t.def("add_line", &TimedGeometryBuilder::add_line);
	t.def("add_sphere", &TimedGeometryBuilder::add_sphere);
	t.def("add_cube", &TimedGeometryBuilder::add_cube);
	t.def("build_data", &TimedGeometryBuilder::build_data);
}

void wrap_geometry_builder(py::module_& m) {
	wrap_GeometryMeshData(m);
	wrap_GeometryBuilder(m);
	wrap_TimedGeometryBuilder(m);
}
