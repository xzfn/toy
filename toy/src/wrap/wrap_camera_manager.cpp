
#include "wrap_camera_manager.h"

#include <pybind11/functional.h>
namespace py = pybind11;

#include "camera_manager.h"


void wrap_camera_manager(pybind11::module_& m) {
	py::class_<CameraManager> t(m, "CameraManager");
	t.def("get_camera", &CameraManager::get_camera, py::return_value_policy::reference);
	t.def("get_camera_controller", &CameraManager::get_camera_controller, py::return_value_policy::reference);
	t.def("is_controlled", &CameraManager::is_controlled);
	t.def("set_is_controlled", &CameraManager::set_is_controlled);
}
