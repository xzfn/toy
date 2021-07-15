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

	t.def("get_transform", &Light::get_transform);
	t.def("set_transform", &Light::set_transform);

	t.def("get_color", &Light::get_color);
	t.def("set_color", &Light::set_color);

	t.def("get_range", &Light::get_range);
	t.def("set_range", &Light::set_range);

	t.def("get_spot_inner_angle", &Light::get_spot_inner_angle);
	t.def("set_spot_inner_angle", &Light::set_spot_inner_angle);

	t.def("get_spot_outer_angle", &Light::get_spot_outer_angle);
	t.def("set_spot_outer_angle", &Light::set_spot_outer_angle);
}

void wrap_light(pybind11::module_& m) {
	wrap_LightType(m);
	wrap_Light(m);
}
