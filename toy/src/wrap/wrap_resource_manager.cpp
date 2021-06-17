#include "wrap_resource_manager.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "resource_manager.h"


void wrap_resource_manager(pybind11::module_& m) {
	py::class_<ResourceManager> t(m, "ResourceManager");
	t.def("get_resource_directories", &ResourceManager::get_resource_directories);
	t.def("set_resource_directories", &ResourceManager::set_resource_directories);
	t.def("full_path", &ResourceManager::full_path);
	t.def("exists", &ResourceManager::exists);
	t.def("read_binary",  &ResourceManager::read_binary);
	t.def("read_text", &ResourceManager::read_text);
}
