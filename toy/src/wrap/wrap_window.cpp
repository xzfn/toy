#include "wrap_window.h"

#include <pybind11/functional.h>
namespace py = pybind11;

#include "window.h"


void wrap_window(pybind11::module_& m) {
	py::class_<Window> t(m, "Window");

	t.def("get_win32_hwnd", [](Window* this_) {
		return (uintptr_t)this_->get_win32_hwnd();
	});
	t.def("get_win32_hinstance", [](Window* this_) {
		return (uintptr_t)this_->get_win32_hinstance();
	});
	t.def("move_window", &Window::move_window);
	t.def("set_borderless", &Window::set_borderless);
	t.def("set_win32_hwnd_parent", [](Window* this_, uintptr_t hwnd_parent) {
		this_->set_win32_hwnd_parent((HWND)hwnd_parent);
	});
}
