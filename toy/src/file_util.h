#pragma once

#include <vector>
#include <string>


namespace fileutil
{
std::vector<uint8_t> read_binary_file(std::string filename);
std::string read_text_file(std::string filename);
}
