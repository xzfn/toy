#include "wrap_camera.h"

#include <pybind11/functional.h>
namespace py = pybind11;

#include "camera.h"

void wrap_camera(pybind11::module_& m) {
	py::class_<Camera> t(m, "Camera");
	t.def("set_view", &Camera::set_view);
	t.def("set_perspective_param", &Camera::set_perspective_param);
	t.def("set_orthographic_param", &Camera::set_orthographic_param);
	t.def("is_perspective", &Camera::is_perspective);
}
