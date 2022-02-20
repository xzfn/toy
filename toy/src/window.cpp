#include "window.h"

#include <cassert>
#include <iostream>
#include <map>


class WindowRegistry {
public:
	void register_window(HWND hwnd, Window* window) {
		assert(m_window_map.find(hwnd) == m_window_map.end());
		m_window_map[hwnd] = window;
	}

	void unregister_window(HWND hwnd) {
		assert(m_window_map.find(hwnd) != m_window_map.end());
		m_window_map.erase(hwnd);
	}

	Window* find_window(HWND hwnd) {
		auto it = m_window_map.find(hwnd);
		if (it != m_window_map.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

private:
	std::map<HWND, Window*> m_window_map;
};

WindowRegistry g_window_registry;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static HINSTANCE get_hinstance() {
	return GetModuleHandle(NULL);
}

Window::Window()
{
}

void Window::init(uint32_t width, uint32_t height, const wchar_t* title)
{

	// Register the window class.
	const wchar_t CLASS_NAME[] = L"win";

	WNDCLASS wc = { };

	HINSTANCE hInstance = get_hinstance();

	RECT rect = {
		0, 0, (LONG)width, (LONG)height
	};
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	// Creating the window.
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		title,    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL) {
		assert(false);
	}

	// register hwnd
	g_window_registry.register_window(hwnd, this);

	// Show the window.
	ShowWindow(hwnd, SW_SHOW);

	// Set to member.
	m_hwnd = hwnd;
	m_hinstance = hInstance;

	m_window_width = width;
	m_window_height = height;
}

HWND Window::get_win32_hwnd()
{
	return m_hwnd;
}

HINSTANCE Window::get_win32_hinstance()
{
	return m_hinstance;
}

void Window::move_window(int x, int y, int width, int height)
{
	SetWindowPos(m_hwnd, NULL, x, y, width, height, SWP_NOZORDER);
}

void Window::set_borderless(bool is_borderless)
{
	if (is_borderless) {
		SetWindowLongPtr(m_hwnd, GWL_STYLE, 0);
	}
	else {
		SetWindowLongPtr(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	}
	SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_SHOWWINDOW);
}

void Window::set_win32_hwnd_parent(HWND hwnd_parent)
{
	SetWindowLongPtr(m_hwnd, GWLP_HWNDPARENT, (LONG_PTR)hwnd_parent);
	SetWindowPos(m_hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

std::pair<uint32_t, uint32_t> Window::get_window_size()
{
	return std::make_pair(m_window_width, m_window_height);
}

Window::~Window()
{
	g_window_registry.unregister_window(m_hwnd);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Window* window = g_window_registry.find_window(hWnd);

	if (window != nullptr) {
		return window->window_proc_impl(hWnd, message, wParam, lParam);
	}
	else {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void Window::set_update_callback(UpdateCallback callback)
{
	m_update_callback = callback;
}

void Window::set_resize_callback(ResizeCallback callback)
{
	m_callbacks.resize = callback;
}

void Window::set_mouse_move_callback(MouseMoveCallback callback)
{
	m_callbacks.mouse_move = callback;
}

void Window::set_mouse_down_callback(MouseDownCallback callback)
{
	m_callbacks.mouse_down = callback;
}

void Window::set_mouse_up_callback(MouseUpCallback callback)
{
	m_callbacks.mouse_up = callback;
}

void Window::set_mouse_wheel_callback(MouseWheelCallback callback)
{
	m_callbacks.mouse_wheel = callback;
}

void Window::set_key_down_callback(KeyDownCallback callback)
{
	m_callbacks.key_down = callback;
}

void Window::set_key_up_callback(KeyUpCallback callback)
{
	m_callbacks.key_up = callback;
}

void Window::set_char_callback(CharCallback callback)
{
	m_callbacks.on_char = callback;
}

LRESULT Window::window_proc_impl(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		SetCapture(hwnd);
		if (m_callbacks.mouse_down) {
			POINTS pt = MAKEPOINTS(lparam);
			m_callbacks.mouse_down(MouseButtonLeft, pt.x, pt.y);
		}
	}
	break;
	case WM_RBUTTONDOWN:
	{
		SetCapture(hwnd);
		if (m_callbacks.mouse_down) {
			POINTS pt = MAKEPOINTS(lparam);
			m_callbacks.mouse_down(MouseButtonRight, pt.x, pt.y);
		}
	}
	break;
	case WM_MBUTTONDOWN:
	{
		SetCapture(hwnd);
		if (m_callbacks.mouse_down) {
			POINTS pt = MAKEPOINTS(lparam);
			m_callbacks.mouse_down(MouseButtonMiddle, pt.x, pt.y);
		}
	}
	break;

	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		if (m_callbacks.mouse_up) {
			POINTS pt = MAKEPOINTS(lparam);
			m_callbacks.mouse_up(MouseButtonLeft, pt.x, pt.y);
		}
	}
	break;
	case WM_RBUTTONUP:
	{
		ReleaseCapture();
		if (m_callbacks.mouse_up) {
			POINTS pt = MAKEPOINTS(lparam);
			m_callbacks.mouse_up(MouseButtonRight, pt.x, pt.y);
		}
	}
	break;
	case WM_MBUTTONUP:
	{
		ReleaseCapture();
		if (m_callbacks.mouse_up) {
			POINTS pt = MAKEPOINTS(lparam);
			m_callbacks.mouse_up(MouseButtonMiddle, pt.x, pt.y);
		}
	}
	break;

	case WM_MOUSEMOVE:
	{
		if (m_callbacks.mouse_move) {
			POINTS pt = MAKEPOINTS(lparam);
			m_callbacks.mouse_move(pt.x, pt.y);
		}
	}
	break;

	case WM_MOUSEWHEEL:
	{
		if (m_callbacks.mouse_wheel) {
			float wheel_delta_scale = 1.0f / 120.0f;
			int32_t delta = GET_WHEEL_DELTA_WPARAM(wparam);
			POINTS pt = MAKEPOINTS(lparam);
			float float_delta = delta * wheel_delta_scale;
			m_callbacks.mouse_wheel(float_delta, pt.x, pt.y);
		}
	}
	break;

	case WM_SIZE:
	{
		if (m_callbacks.resize) {
			UINT width = LOWORD(lparam);
			UINT height = HIWORD(lparam);
			m_window_width = width;
			m_window_height = height;
			m_callbacks.resize(width, height);
		}
	}
	break;
	
	case WM_KEYDOWN:
	{
		if (m_callbacks.key_down) {
			if (lparam & (1 << 30)) {
				// is repeat
			}
			else {
				// virtual key codes
				// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
				uint32_t key = (uint32_t)wparam;
				m_callbacks.key_down(key);
			}
		}
	}
	break;

	case WM_KEYUP:
	{
		if (m_callbacks.key_up) {
			uint32_t key = (uint32_t)wparam;
			m_callbacks.key_up(key);
		}
	}
	break;

	case WM_CHAR:
	{
		if (m_callbacks.on_char) {
			uint32_t c = (uint32_t)wparam;
			m_callbacks.on_char(c);
		}
	}
	break;

	case WM_DESTROY:
	{
		std::cout << "WM_DESTROY\n";
		PostQuitMessage(0);
		m_should_close = true;
	}
	break;
	
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
	return 0;
}

void Window::poll_events()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Window::set_should_close()
{
	m_should_close = true;
}

bool Window::should_close()
{
	return m_should_close;
}

int Window::mainloop()
{
	while (!should_close()) {
		poll_events();
		if (should_close()) {
			break;
		}
		else {
			if (m_update_callback) {
				m_update_callback();
			}
		}
	}
	return 0;
}
