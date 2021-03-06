#pragma once

#include <cstdint>
#include <utility>
#include <map>
#include <set>


typedef uint32_t Key;
typedef uint32_t MouseButton;
struct MousePosition {
	int x = 0;
	int y = 0;
};
struct MouseMove {
	int dx = 0;
	int dy = 0;
};

class InputManager {
public:
	InputManager();
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;

public:
	void input_key_down(Key key);
	void input_key_up(Key key);
	void input_mouse_button_down(MouseButton mouse_button);
	void input_mouse_button_up(MouseButton mouse_button);
	void input_mouse_move(int x, int y);
	void input_mouse_wheel(float wheel_delta);

	void clear_frame();

public:
	bool get_key(Key key);
	bool get_key_down(Key key);
	bool get_key_up(Key key);
	bool get_mouse_button(MouseButton mouse_button);
	bool get_mouse_button_down(MouseButton mouse_button);
	bool get_mouse_button_up(MouseButton mouse_button);
	MousePosition get_mouse_position();
	MouseMove get_mouse_move();
	float get_mouse_wheel();

private:
	std::set<Key> m_frame_key_down;
	std::set<Key> m_frame_key_up;
	std::map<Key, bool> m_key_states;

	std::set<Key> m_frame_mouse_button_down;
	std::set<Key> m_frame_mouse_button_up;
	std::map<MouseButton, bool> m_mouse_button_states;

	MousePosition m_mouse_position;
	MouseMove m_frame_mouse_move;

	float m_frame_mouse_wheel_delta = 0.0f;
};
