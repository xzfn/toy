#include "wrap_mesh.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "mesh.h"


void wrap_mesh(pybind11::module_& m) {
	py::class_<Mesh> t(m, "Mesh");
	t.def("init_resource", &Mesh::init_resource);
}
