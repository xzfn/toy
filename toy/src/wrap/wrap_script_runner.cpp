#include "wrap_script_runner.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "script_runner.h"


void wrap_script_runner(pybind11::module_& m) {
	py::class_<ScriptRunner> t(m, "ScriptRunner");
	t.def("lua_script", &ScriptRunner::lua_script);
	t.def("py_script", &ScriptRunner::py_script);
}
