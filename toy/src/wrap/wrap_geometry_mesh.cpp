#include "wrap_geometry_mesh.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "geometry_mesh.h"
#include "load_helper.h"


void wrap_geometry_mesh(pybind11::module_& m) {
	py::class_<GeometryMesh, std::shared_ptr<GeometryMesh>> t(m, "GeometryMesh");
	t.def_static("create", &create_geometry_mesh);
}
