#pragma once

#include <functional>
#include <exception>
#include <cstdlib>

#include <pybind11/pybind11.h>


// safe run
#define PYUTIL_SAFE_RUN_BEGIN \
	try {


#define PYUTIL_SAFE_RUN_END \
	}\
	catch (pybind11::error_already_set& e) {\
		pyutil::process_pybind11_exception(e);\
	}


#define PYUTIL_SAFE_RUN(stmt) \
	PYUTIL_SAFE_RUN_BEGIN\
	stmt;\
	PYUTIL_SAFE_RUN_END


// check run
#define PYUTIL_CHECK_RUN_BEGIN \
	try {


#define PYUTIL_CHECK_RUN_END \
	}\
	catch (pybind11::error_already_set& e) {\
		pyutil::process_pybind11_exception(e);\
		std::abort();\
	}


#define PYUTIL_CHECK_RUN(stmt) \
	PYUTIL_CHECK_RUN_BEGIN\
	stmt;\
	PYUTIL_CHECK_RUN_END


namespace pyutil {

void process_pybind11_exception(pybind11::error_already_set& e);

void print_pybind11_exception(pybind11::error_already_set& e);

void call_sys_excepthook(pybind11::error_already_set& e);


// safe run std::function
inline void safe_call(std::function<void()> func) {
	PYUTIL_SAFE_RUN_BEGIN
	func();
	PYUTIL_SAFE_RUN_END
}


struct SafeCallWrapper {
	SafeCallWrapper(std::function<void()> func)
		:m_func(func)
	{ }

	void operator()() {
		safe_call(m_func);
	}

	std::function<void()> m_func;
};


template <typename R, typename ...Args>
R generic_safe_call(std::function<R(Args...)> func, Args... args) {
	PYUTIL_SAFE_RUN_BEGIN
	return func(args...);
	PYUTIL_SAFE_RUN_END
}


template <typename> struct GenericSafeCallWrapper;

template <typename R, typename ...Args>
struct GenericSafeCallWrapper<R(Args...)> {
	GenericSafeCallWrapper(std::function<R(Args...)> func)
		:m_func(func)
	{ }

	R operator()(Args... args) {
		PYUTIL_SAFE_RUN_BEGIN
		return m_func(args...);
		PYUTIL_SAFE_RUN_END
	}

	std::function<R(Args...)> m_func;
};

}
