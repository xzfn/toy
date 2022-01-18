#include "luawrap_input_manager.h"

#include "input_manager.h"


void luawrap_input_manager(sol::table& m) {
	auto t = m.new_usertype<InputManager>("InputManager");
	t["new"] = sol::no_constructor;

	t.set_function("get_key", &InputManager::get_key);
	t.set_function("get_key_down", &InputManager::get_key_down);
	t.set_function("get_key_up", &InputManager::get_key_up);

	t.set_function("get_mouse_button", &InputManager::get_mouse_button);
	t.set_function("get_mouse_button_down", &InputManager::get_mouse_button_down);
	t.set_function("get_mouse_button_up", &InputManager::get_mouse_button_up);
	t.set_function("get_mouse_position", [](InputManager& input_manager) {
		auto mouse_position = input_manager.get_mouse_position();
		return std::make_pair(mouse_position.x, mouse_position.y);
	});
	t.set_function("get_mouse_move", [](InputManager& input_manager){
		auto mouse_move = input_manager.get_mouse_move();
		return std::make_pair(mouse_move.dx, mouse_move.dy);
	});
	t.set_function("get_mouse_wheel", &InputManager::get_mouse_wheel);

	t.set_function("get_joystick_button", &InputManager::get_joystick_button);
	t.set_function("get_joystick_button_down", &InputManager::get_joystick_button_down);
	t.set_function("get_joystick_button_up", &InputManager::get_joystick_button_up);
	t.set_function("get_joystick_trigger_left", &InputManager::get_joystick_trigger_left);
	t.set_function("get_joystick_trigger_right", &InputManager::get_joystick_trigger_right);
	t.set_function("get_joystick_thumb_left", [](InputManager& input_manager) {
		auto thumb_position = input_manager.get_joystick_thumb_left();
		return std::make_pair(thumb_position.x, thumb_position.y);
	});
	t.set_function("get_joystick_thumb_right", [](InputManager& input_manager) {
		auto thumb_position = input_manager.get_joystick_thumb_right();
		return std::make_pair(thumb_position.x, thumb_position.y);
	});
}
