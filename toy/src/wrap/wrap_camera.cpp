#include "wrap_camera.h"

#include <pybind11/functional.h>
namespace py = pybind11;

#include "camera.h"

void wrap_PerspectiveData(py::module_& m) {
	py::class_<PerspectiveData> t(m, "PerspectiveData");
	t.def(py::init<>());
	t.def_readwrite("fov", &PerspectiveData::fov);
	t.def_readwrite("aspect", &PerspectiveData::aspect);
	t.def_readwrite("z_near", &PerspectiveData::z_near);
	t.def_readwrite("z_far", &PerspectiveData::z_far);
}

void wrap_OrthographicData(py::module_& m) {
	py::class_<OrthographicData> t(m, "OrthographicData");
	t.def(py::init<>());
	t.def_readwrite("left", &OrthographicData::left);
	t.def_readwrite("right", &OrthographicData::right);
	t.def_readwrite("bottom", &OrthographicData::bottom);
	t.def_readwrite("top", &OrthographicData::top);
	t.def_readwrite("z_near", &OrthographicData::z_near);
	t.def_readwrite("z_far", &OrthographicData::z_far);
}

void wrap_Camera(py::module_& m) {
	py::class_<Camera> t(m, "Camera");
	t.def("set_view", &Camera::set_view);
	t.def("set_perspective_param", &Camera::set_perspective_param);
	t.def("set_orthographic_param", &Camera::set_orthographic_param);
	t.def("set_perspective_data", &Camera::set_perspective_data);
	t.def("set_orthographic_data", &Camera::set_orthographic_data);
	t.def("get_perspective_data", &Camera::get_perspective_data);
	t.def("get_orthographic_data", &Camera::get_orthographic_data);
	t.def("is_perspective", &Camera::is_perspective);
	t.def("set_perspective", &Camera::set_perspective);
	t.def("get_view", &Camera::get_view);
	t.def("get_projection", &Camera::get_projection);
	t.def("get_view_projection", &Camera::get_view_projection);
	t.def("screen_to_world", &Camera::screen_to_world);
	t.def("screen_to_world_ray", &Camera::screen_to_world_ray);
	t.def("world_to_screen", &Camera::world_to_screen);
}

void wrap_camera(pybind11::module_& m) {
	wrap_PerspectiveData(m);
	wrap_OrthographicData(m);
	wrap_Camera(m);
}
