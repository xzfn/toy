#include "wrap_light_manager.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "light_manager.h"


void wrap_light_manager(pybind11::module_& m) {
	py::class_<LightManager> t(m, "LightManager");
	t.def("add_light", &LightManager::add_light);
	t.def("remove_light", &LightManager::remove_light);
	t.def("set_sun", &LightManager::set_sun);
	t.def("get_sun", &LightManager::get_sun);
}
