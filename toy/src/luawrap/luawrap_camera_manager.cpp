
#include "luawrap_camera_manager.h"

#include "camera_manager.h"


void luawrap_camera_manager(sol::table& m) {
	auto t = m.new_usertype<CameraManager>("CameraManager");
	t.set_function("get_camera", &CameraManager::get_camera);
	t.set_function("get_camera_controller", &CameraManager::get_camera_controller);
	t.set_function("is_controlled", &CameraManager::is_controlled);
	t.set_function("set_is_controlled", &CameraManager::set_is_controlled);
}
