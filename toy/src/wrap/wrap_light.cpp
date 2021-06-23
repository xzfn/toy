#include "wrap_light.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "light.h"


void wrap_LightType(py::module_& m) {
	py::enum_<LightType> t(m, "LightType");
	t.value("Empty", LightType::Empty);
	t.value("Point", LightType::Point);
	t.value("Spot", LightType::Spot);
	t.value("Directional", LightType::Directional);
}

void wrap_Light(py::module_& m) {
	py::class_<Light, std::shared_ptr<Light>> t(m, "Light");
	t.def(py::init<>());

	t.def("get_type", &Light::get_type);
	t.def("set_type", &Light::set_type);

	t.def("get_position", &Light::get_position);
	t.def("set_position", &Light::set_position);

	t.def("get_color", &Light::get_color);
	t.def("set_color", &Light::set_color);

	t.def("get_direction", &Light::get_direction);
	t.def("set_direction", &Light::set_direction);
}

void wrap_light(pybind11::module_& m) {
	wrap_LightType(m);
	wrap_Light(m);
}
