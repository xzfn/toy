#pragma once

#include <cstdint>
#include <vector>
#include <string>


namespace imageutil
{

struct Image {
	int width;
	int height;
	int channels;
	std::vector<uint8_t> data;
};

Image load_image(std::string filename);
Image load_image_force_channels(std::string filename, int force_channels);
Image create_color_image(int width, int height, int channels, uint32_t pixel);





}