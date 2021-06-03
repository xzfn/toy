#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>
#include <functional>


constexpr uint32_t MouseButtonLeft = 0;
constexpr uint32_t MouseButtonRight = 1;
constexpr uint32_t MouseButtonMiddle = 2;


typedef std::function<void(uint32_t width, uint32_t height)> ResizeCallback;
typedef std::function<void(uint32_t x, uint32_t y)> MouseMoveCallback;
typedef std::function<void(uint32_t mouse_button, uint32_t x, uint32_t y)> MouseDownCallback;
typedef std::function<void(uint32_t mouse_button, uint32_t x, uint32_t y)> MouseUpCallback;
typedef std::function<void(float delta, uint32_t x, uint32_t y)> MouseWheelCallback;
typedef std::function<void(uint32_t key)> KeyDownCallback;
typedef std::function<void(uint32_t key)> KeyUpCallback;

typedef std::function<void()> UpdateCallback;


class Window {
public:
	Window();
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void init(uint32_t width, uint32_t height, const wchar_t* title);
	
	HWND get_win32_hwnd();
	HINSTANCE get_win32_hinstance();

	void set_update_callback(UpdateCallback callback);

	void set_resize_callback(ResizeCallback callback);
	void set_mouse_move_callback(MouseMoveCallback callback);
	void set_mouse_down_callback(MouseDownCallback callback);
	void set_mouse_up_callback(MouseUpCallback callback);
	void set_mouse_wheel_callback(MouseWheelCallback callback);
	void set_key_down_callback(KeyDownCallback callback);
	void set_key_up_callback(KeyUpCallback callback);

	void set_should_close();

	void poll_events();
	bool should_close();

	int mainloop();

public:
	LRESULT window_proc_impl(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private:
	struct Callbacks {
		ResizeCallback resize;
		MouseMoveCallback mouse_move;
		MouseDownCallback mouse_down;
		MouseUpCallback mouse_up;
		MouseWheelCallback mouse_wheel;
		KeyDownCallback key_down;
		KeyUpCallback key_up;
	};

private:
	std::function<void()> m_update_callback;
	Callbacks m_callbacks;
	HWND m_hwnd = NULL;
	HINSTANCE m_hinstance = NULL;
	bool m_should_close = false;
};
