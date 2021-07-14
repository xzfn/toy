#include "wrap_camera_controller.h"

#include <pybind11/functional.h>
namespace py = pybind11;

#include "camera_controller.h"


void wrap_CameraController(py::module_& m) {
	py::class_<CameraController> t(m, "CameraController");
	t.def("translate", &CameraController::translate);
	t.def("rotate", &CameraController::rotate);
	t.def("set_transform", &CameraController::set_transform);
	t.def("get_transform", &CameraController::get_transform);
}

void wrap_camera_controller(pybind11::module_& m) {
	wrap_CameraController(m);
}
