#include "wrap_basic_pipeline.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "basic_pipeline.h"
#include "load_helper.h"


void wrap_basic_pipeline(pybind11::module_& m) {
	py::class_<BasicPipeline, std::shared_ptr<BasicPipeline>> t(m, "BasicPipeline");
	t.def_static("create", &create_basic_pipeline);
	t.def("get_shader_spvs", &BasicPipeline::get_shader_spvs);
	t.def("reload_shader", &BasicPipeline::reload_shader);
	// t.def("init_resource", &BasicPipeline::init_resource);
}
