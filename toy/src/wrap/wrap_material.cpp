#include "wrap_material.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "material.h"
#include "load_helper.h"


void wrap_material(pybind11::module_& m) {
	py::class_<Material, std::shared_ptr<Material>> t(m, "Material");
	t.def_static("create", &create_material);
	t.def("set_base_color", [](Material* this_, glm::vec3 color) {
		MaterialUniforms& uniforms = this_->ref_uniforms();
		uniforms.base_color = color;
	});
}
