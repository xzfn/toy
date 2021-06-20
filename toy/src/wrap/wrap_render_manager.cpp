#include "wrap_render_manager.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "render_manager.h"


void wrap_render_manager(pybind11::module_& m) {
	py::class_<RenderManager> t(m, "RenderManager");
	t.def("add_mesh", &RenderManager::add_mesh);
	t.def("add_geometry_mesh", &RenderManager::add_geometry_mesh);
}
