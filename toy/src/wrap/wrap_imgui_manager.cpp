#include "wrap_imgui_manager.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "imgui_manager.h"


void wrap_ImGuiManager(pybind11::module_& m) {
	py::class_<ImGuiManager> t(m, "ImGuiManager");
	t.def("add_imgui_mesh_part", &ImGuiManager::add_imgui_mesh_part);
}

void wrap_imgui_manager(pybind11::module_& m) {
	wrap_ImGuiManager(m);
}
