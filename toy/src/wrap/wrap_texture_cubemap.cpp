#include "wrap_texture_cubemap.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "texture_cubemap.h"
#include "load_helper.h"


void wrap_texture_cubemap(pybind11::module_& m) {
	py::class_<TextureCubemap> t(m, "TextureCubemap");
	t.def_static("create", &create_texture_cubemap);
}
