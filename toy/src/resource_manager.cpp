#include "resource_manager.h"

#include <iostream>

#include "file_util.h"



ResourceManager g_resource_manager;

ResourceManager::ResourceManager()
{
	add_resource_directory(".");
}

void ResourceManager::add_resource_directory(std::string directory)
{
	m_resource_directories.push_back(directory);
}

std::vector<std::string> ResourceManager::get_resouce_directories()
{
	return m_resource_directories;
}

std::string ResourceManager::full_path(std::string path)
{
	for (auto& directory : m_resource_directories) {
		std::string filename = directory + "/" + path;
		std::ifstream is(filename, std::ios_base::binary);
		if (is) {
			return filename;
		}
	}
	std::cout << "WARN full_path failed: " << path << "\n";
	return "";
}

bool ResourceManager::exists(std::string path)
{
	std::ifstream is = open_file(path, std::ios_base::binary);
	if (is) {
		return true;
	}
	else {
		return false;
	}
}

std::ifstream ResourceManager::open_file(std::string path, std::ios_base::openmode mode)
{
	for (auto& directory : m_resource_directories) {
		std::string filename = directory + "/" + path;
		std::ifstream is(filename, mode);
		if (is) {
			return is;
		}
	}
	std::cout << "WARN open_file failed: " << path << "\n";
	return std::ifstream();
}

std::vector<uint8_t> ResourceManager::read_binary(std::string path)
{
	return fileutil::read_binary_file(full_path(path));
}

std::string ResourceManager::read_text(std::string path)
{
	return fileutil::read_text_file(full_path(path));
}

ResourceManager& get_resource_manager()
{
	return g_resource_manager;
}
