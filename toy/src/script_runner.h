#pragma once

#include <string>


class ScriptRunner {
public:
	void lua_script(std::string code);
	void py_script(std::string code);

};
