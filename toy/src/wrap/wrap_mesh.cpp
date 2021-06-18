#include "wrap_mesh.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "mesh.h"
#include "load_helper.h"


void wrap_mesh(pybind11::module_& m) {
	py::class_<Mesh, std::shared_ptr<Mesh>> t(m, "Mesh");
	t.def_static("create", &create_mesh);
}
