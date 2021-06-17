#include "wrap_basic_pipeline.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "basic_pipeline.h"


void wrap_basic_pipeline(pybind11::module_& m) {
	py::class_<BasicPipeline> t(m, "BasicPipeline");
	// t.def("init_resource", &BasicPipeline::init_resource);
}
