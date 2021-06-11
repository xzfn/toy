#include "wrap_material.h"

#include <pybind11/stl.h>
namespace py = pybind11;

#include "material.h"


void wrap_material(pybind11::module_& m) {
	py::class_<Material> t(m, "Material");
	// t.def("init", &Material::init);
}
