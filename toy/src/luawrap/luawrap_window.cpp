#include "luawrap_window.h"

#include "window.h"


void luawrap_window(sol::table& m) {
	auto t = m.new_usertype<Window>("Window");
	t[sol::meta_function::construct] = sol::no_constructor;

	t.set_function("get_win32_hwnd", [](Window* this_) {
		return (uintptr_t)this_->get_win32_hwnd();
	});
	t.set_function("get_win32_hinstance", [](Window* this_) {
		return (uintptr_t)this_->get_win32_hinstance();
	});
	t.set_function("move_window", &Window::move_window);
	t.set_function("set_borderless", &Window::set_borderless);
	t.set_function("set_win32_hwnd_parent", [](Window* this_, uintptr_t hwnd_parent) {
		this_->set_win32_hwnd_parent((HWND)hwnd_parent);
	});
}
