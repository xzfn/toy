#pragma once

#include <cstdint>
#include <fstream>
#include <vector>
#include <string>


class ResourceManager {
public:
	ResourceManager();

	void add_resource_directory(std::string directory);

	std::vector<std::string> get_resource_directories();
	void set_resource_directories(std::vector<std::string> directories);

	std::string full_path(std::string path);
	bool exists(std::string path);
	std::ifstream open_file(std::string path, std::ios_base::openmode mode);

	std::vector<uint8_t> read_binary(std::string path);
	std::string read_text(std::string path);

private:
	std::vector<std::string>  m_resource_directories;
};
