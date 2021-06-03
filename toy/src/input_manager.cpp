#include "input_manager.h"

InputManager::InputManager()
{
}

void InputManager::input_key_down(Key key)
{
	m_frame_key_down.insert(key);
	m_key_states[key] = true;
}

void InputManager::input_key_up(Key key)
{
	m_frame_key_up.insert(key);
	m_key_states[key] = false;
}

void InputManager::input_mouse_button_down(MouseButton mouse_button)
{
	m_frame_mouse_button_down.insert(mouse_button);
	m_mouse_button_states[mouse_button] = true;
}

void InputManager::input_mouse_button_up(MouseButton mouse_button)
{
	m_frame_mouse_button_up.insert(mouse_button);
	m_mouse_button_states[mouse_button] = false;
}

void InputManager::input_mouse_move(int x, int y)
{
	m_frame_mouse_move.dx += x - m_mouse_position.x;
	m_frame_mouse_move.dy += y - m_mouse_position.y;
	m_mouse_position.x = x;
	m_mouse_position.y = y;
}

void InputManager::input_mouse_wheel(float wheel_delta)
{
	m_frame_mouse_wheel_delta += wheel_delta;
}

void InputManager::clear_frame()
{
	m_frame_key_down.clear();
	m_frame_key_up.clear();
	m_frame_mouse_button_down.clear();
	m_frame_mouse_button_up.clear();
	m_frame_mouse_move.dx = 0;
	m_frame_mouse_move.dy = 0;
	m_frame_mouse_wheel_delta = 0.0f;
}

bool InputManager::get_key(Key key)
{
	auto it = m_key_states.find(key);
	if (it == m_key_states.end()) {
		return false;
	}
	return m_key_states[key];
}

bool InputManager::get_key_down(Key key)
{
	return m_frame_key_down.find(key) != m_frame_key_down.end();
}

bool InputManager::get_key_up(Key key)
{
	return m_frame_key_up.find(key) != m_frame_key_up.end();
}

bool InputManager::get_mouse_button(MouseButton mouse_button)
{
	auto it = m_mouse_button_states.find(mouse_button);
	if (it == m_mouse_button_states.end()) {
		return false;
	}
	return m_mouse_button_states[mouse_button];
}

bool InputManager::get_mouse_button_down(MouseButton mouse_button)
{
	return m_frame_mouse_button_down.find(mouse_button) != m_frame_mouse_button_down.end();
}

bool InputManager::get_mouse_button_up(MouseButton mouse_button)
{
	return m_frame_mouse_button_up.find(mouse_button) != m_frame_mouse_button_up.end();
}

MousePosition InputManager::get_mouse_position()
{
	return m_mouse_position;
}

MouseMove InputManager::get_mouse_move()
{
	return m_frame_mouse_move;
}

float InputManager::get_mouse_wheel()
{
	return m_frame_mouse_wheel_delta;
}
