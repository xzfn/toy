#include "wrap_texture.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "texture.h"
#include "load_helper.h"


void wrap_texture(pybind11::module_& m) {
	py::class_<Texture, std::shared_ptr<Texture>> t(m, "Texture");
	t.def_static("create", &create_texture);
	t.def_static("create_color", &create_color_texture);
}
