#include "input_manager.h"

#include <windows.h>
#include "Xinput.h"
#include "winerror.h"

#include <cassert>


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

void InputManager::input_char(uint32_t c)
{
	m_frame_chars.push_back(c);
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

void InputManager::set_frame_capture_keyboard()
{
	m_frame_capture_keyboard = true;
}

void InputManager::clear_frame()
{
	m_frame_key_down.clear();
	m_frame_key_up.clear();
	m_frame_chars.clear();
	m_frame_mouse_button_down.clear();
	m_frame_mouse_button_up.clear();
	m_frame_mouse_move.dx = 0;
	m_frame_mouse_move.dy = 0;
	m_frame_mouse_wheel_delta = 0.0f;
	m_frame_joystick_button_down = 0;
	m_frame_joystick_button_up = 0;
	m_frame_capture_keyboard = false;
	update_joystick();
}

bool InputManager::get_key(Key key)
{
	if (m_frame_capture_keyboard) {
		return false;
	}
	auto it = m_key_states.find(key);
	if (it == m_key_states.end()) {
		return false;
	}
	return m_key_states[key];
}

bool InputManager::get_key_down(Key key)
{
	if (m_frame_capture_keyboard) {
		return false;
	}
	return m_frame_key_down.find(key) != m_frame_key_down.end();
}

bool InputManager::get_key_up(Key key)
{
	if (m_frame_capture_keyboard) {
		return false;
	}
	return m_frame_key_up.find(key) != m_frame_key_up.end();
}

std::vector<Key> InputManager::get_keys_down()
{
	return std::vector(m_frame_key_down.begin(), m_frame_key_down.end());
}

std::vector<Key> InputManager::get_keys_up()
{
	return std::vector(m_frame_key_up.begin(), m_frame_key_up.end());
}

std::vector<uint32_t> InputManager::get_chars()
{
	return m_frame_chars;
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

void InputManager::update_joystick()
{
	DWORD res;
	XINPUT_STATE state{0};
	int controller_index = 0;
	res = XInputGetState(controller_index, &state);
	if (res == ERROR_SUCCESS) {
		// controller is connected
		uint64_t new_packet_number = state.dwPacketNumber;
		if (m_joystick_packet_number != new_packet_number) {
			m_joystick_packet_number = new_packet_number;
			XINPUT_GAMEPAD gamepad = state.Gamepad;
			input_joystick_state(
				gamepad.wButtons,
				gamepad.bLeftTrigger / 255.0f,
				gamepad.bRightTrigger / 255.0f,
				gamepad.sThumbLX / 32768.0f,
				gamepad.sThumbLY / 32768.0f,
				gamepad.sThumbRX / 32768.0f,
				gamepad.sThumbRY / 32768.0f
			);
		}
	}
	else {
		// controller is not connected

	}
}

void InputManager::input_joystick_state(
	uint32_t buttons,
	float trigger_left, float trigger_right,
	float thumb_left_x, float thumb_left_y,
	float thumb_right_x, float thumb_right_y
) {
	uint32_t old_buttons = m_joystick_button_states;
	uint32_t changed_buttons = old_buttons ^ buttons;
	for (uint32_t i = 0; i < 16; ++i) {
		uint32_t mask = 1 << i;
		if (changed_buttons & mask) {
			if (buttons & mask) {
				// button down
				m_frame_joystick_button_down |= mask;
			}
			else {
				// button up
				m_frame_joystick_button_up |= mask;
			}
		}
	}
	m_joystick_button_states = buttons;

	m_joystick_trigger_left = trigger_left;
	m_joystick_trigger_right = trigger_right;
	m_joystick_thumb_left = ThumbPosition{thumb_left_x, thumb_left_y};
	m_joystick_thumb_right = ThumbPosition{thumb_right_x, thumb_right_y};
}

bool InputManager::get_joystick_button_down(JoystickButton joystick_button)
{
	return m_frame_joystick_button_down & (1 << joystick_button);
}

bool InputManager::get_joystick_button_up(JoystickButton joystick_button)
{
	return m_frame_joystick_button_up & (1 << joystick_button);
}

bool InputManager::get_joystick_button(JoystickButton joystick_button)
{
	return m_joystick_button_states & (1 << joystick_button);
}

float InputManager::get_joystick_trigger_left()
{
	return m_joystick_trigger_left;
}

float InputManager::get_joystick_trigger_right()
{
	return m_joystick_trigger_right;
}

ThumbPosition InputManager::get_joystick_thumb_left()
{
	return m_joystick_thumb_left;
}

ThumbPosition InputManager::get_joystick_thumb_right()
{
	return m_joystick_thumb_right;
}

void InputManager::set_clipboard_text(std::string text)
{
	// https://stackoverflow.com/questions/1264137/how-to-copy-string-to-clipboard-in-c
	if (text.empty()) {
		return;
	}

	std::size_t len = text.size() + 1;
	HGLOBAL h_mem = GlobalAlloc(GMEM_MOVEABLE, len);
	assert(h_mem != nullptr);
	memcpy(GlobalLock(h_mem), text.data(), len);
	GlobalUnlock(h_mem);

	OpenClipboard(nullptr);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, h_mem);
	CloseClipboard();
}

std::string InputManager::get_clipboard_text()
{
	// https://stackoverflow.com/questions/14762456/getclipboarddatacf-text
	OpenClipboard(nullptr);

	HANDLE h_data = GetClipboardData(CF_TEXT);
	if (h_data == nullptr) {
		CloseClipboard();
		return std::string();
	}
	char* p_text = (char*)GlobalLock(h_data);
	if (p_text == nullptr) {
		CloseClipboard();
		return std::string();
	}

	std::string text(p_text);
	GlobalUnlock(h_data);
	CloseClipboard();
	return text;
}
