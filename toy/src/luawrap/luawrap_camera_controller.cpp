#include "luawrap_camera_controller.h"

#include "camera_controller.h"


void luawrap_CameraController(sol::table& m) {
	auto t = m.new_usertype<CameraController>("CameraController");
	t[sol::meta_function::construct] = sol::no_constructor;
	t.set_function("translate", &CameraController::translate);
	t.set_function("rotate", &CameraController::rotate);
	t.set_function("set_transform", &CameraController::set_transform);
	t.set_function("get_transform", &CameraController::get_transform);
}

void luawrap_camera_controller(sol::table& m) {
	luawrap_CameraController(m);
}
