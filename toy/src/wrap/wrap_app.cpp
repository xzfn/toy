#include "wrap_app.h"

#include <pybind11/functional.h>
namespace py = pybind11;

#include "toyapp.h"


void wrap_app(pybind11::module_& m) {
	py::class_<App> t(m, "App");

	t.def_readonly("delta_time", &App::delta_time);

	t.def_readonly("timer_manager", &App::timer_manager);
	t.def_readonly("timed_text_builder", &App::timed_text_builder);
	t.def_readonly("timed_geometry_builder", &App::timed_geometry_builder);
	t.def_readonly("input_manager", &App::input_manager);
	t.def_readonly("camera_manager", &App::camera_manager);
	t.def_readonly("resource_manager", &App::resource_manager);
	t.def_readonly("render_manager", &App::render_manager);
	t.def_readonly("imgui_manager", &App::imgui_manager);
	t.def_readonly("light_manager", &App::light_manager);
	t.def_readonly("script_runner", &App::script_runner);
	t.def_readonly("material_lines", &App::material_lines);

	t.def("set_background_color", &App::set_background_color);
	t.def("bind_key_down", [](App* this_, KeyDownCallback callback) {
		this_->script_on_key_down = callback;
	});
	t.def("get_window", &App::get_window, py::return_value_policy::reference);
	t.def("set_should_close", &App::set_should_close);
	t.def_readwrite("want_capture_keyboard", &App::want_capture_keyboard);
}
