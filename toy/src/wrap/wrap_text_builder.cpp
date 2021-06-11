#include "wrap_text_builder.h"

#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;

#include "text_builder.h"


void wrap_text_builder(py::module_& m) {
	py::class_<TimedTextBuilder> t(m, "TimedTextBuilder");
	t.def("add_text", &TimedTextBuilder::add_text);
	t.def("add_screen_text", &TimedTextBuilder::add_screen_text);
}
