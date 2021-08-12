#include "wrap_render_manager.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "render_manager.h"


void wrap_PartSlashTotal(pybind11::module_& m) {
	py::class_<PartSlashTotal> t(m, "PartSlashTotal");
	t.def_readonly("part", &PartSlashTotal::part);
	t.def_readonly("total", &PartSlashTotal::total);
}

void wrap_RenderManagerDebugInfo(pybind11::module_& m) {
	py::class_<RenderManagerDebugInfo> t(m, "RenderManagerDebugInfo");
	t.def_readonly("render_culled", &RenderManagerDebugInfo::render_culled);
	t.def_readonly("depth_culled_list", &RenderManagerDebugInfo::depth_culled_list);
}

void wrap_RenderManager(pybind11::module_& m) {
	py::class_<RenderManager> t(m, "RenderManager");
	t.def("add_mesh", &RenderManager::add_mesh);
	t.def("add_geometry_mesh", &RenderManager::add_geometry_mesh);
	t.def("get_debug_info", &RenderManager::get_debug_info);
}

void wrap_render_manager(pybind11::module_& m) {
	wrap_PartSlashTotal(m);
	wrap_RenderManagerDebugInfo(m);
	wrap_RenderManager(m);
}
