#include "file_util.h"

#include <fstream>
#include <iostream>
#include <cassert>

namespace fileutil
{

void report_file_error(std::string filename) {
	std::cout << "ERROR file not found: " << filename << "\n";
	assert(false);
}

void check_ifstream(std::ifstream& is, std::string filename) {
	if (!is) {
		report_file_error(filename);
	}
}

std::vector<uint8_t> read_binary_file(std::string filename) {
	// from std::basic_istream::read
	std::ifstream is(filename, std::ios::binary | std::ios::ate);
	check_ifstream(is, filename);

	auto size = is.tellg();
	std::vector<uint8_t> buffer(size);
	is.seekg(0);
	is.read((char*)buffer.data(), size);
	return buffer;
}

std::string read_text_file(std::string filename)
{
	std::ifstream is(filename, std::ios::ate);
	check_ifstream(is, filename);

	auto size = is.tellg();
	std::string buffer(size, '\0');
	is.seekg(0);
	is.read((char*)buffer.data(), size);
	return buffer;
}




}