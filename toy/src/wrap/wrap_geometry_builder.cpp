#include "wrap_geometry_builder.h"

#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;

#include "geometry_builder.h"


void wrap_geometry_builder(py::module_& m) {
	py::class_<TimedGeometryBuilder> t(m, "TimedGeometryBuilder");
	t.def("add_line", &TimedGeometryBuilder::add_line);
}
