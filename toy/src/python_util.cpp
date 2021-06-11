#include "python_util.h"

#include <iostream>


namespace pyutil {

void process_pybind11_exception(pybind11::error_already_set& e)
{
    print_pybind11_exception(e);
    call_sys_excepthook(e);
}

void print_pybind11_exception(pybind11::error_already_set& e)
{
	std::cout << "ERROR python throws: " << e.what() << "\n";
}

void call_sys_excepthook(pybind11::error_already_set& e) {
	auto& sys = pybind11::module_::import("sys");
	auto& excepthook = sys.attr("excepthook");
	if (excepthook) {
		excepthook(e.type(), e.value(), e.trace());
	}
}

}
