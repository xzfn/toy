#include "wrap_timer_manager.h"

#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;

#include "timer_manager.h"

#include "python_util.h"


void wrap_timer_manager(py::module_& m) {
	py::class_<TimerManager> t(m, "TimerManager");
	t.def(py::init<>());
	t.def("schedule", &TimerManager::schedule);
	t.def("add_timer_unsafe", &TimerManager::add_timer);
	t.def("add_timer", [](TimerManager* this_, double duration, TimerCallback callback) {
		return this_->add_timer(duration, pyutil::SafeCallWrapper(callback));
	});

	t.def("add_repeat_timer_unsafe", &TimerManager::add_repeat_timer);
	t.def("add_repeat_timer", [](TimerManager* this_, double duraiton, TimerCallback callback) {
		return this_->add_repeat_timer(duraiton, pyutil::SafeCallWrapper(callback));
	});

	t.def("cancel_timer", &TimerManager::cancel_timer);
	t.def("clear_timers", &TimerManager::clear_timers);
}
