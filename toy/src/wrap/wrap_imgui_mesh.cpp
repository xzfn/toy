#include "wrap_imgui_mesh.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "imgui_mesh.h"
#include "load_helper.h"


void wrap_imgui_mesh(pybind11::module_& m) {
	py::class_<ImGuiMesh, std::shared_ptr<ImGuiMesh>> t(m, "ImGuiMesh");
	t.def_static("create", &create_imgui_mesh);
}
