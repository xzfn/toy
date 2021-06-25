#include "image_util.h"

#include <iostream>
#include <exception>

#include "stb_image.h"


namespace imageutil
{

Image load_image_channels(std::string filename, int channels)
{
	Image image{};
	int channels_in_file = 0;
	uint8_t* data = stbi_load(
		filename.c_str(), &image.width,
		&image.height, &channels_in_file,
		channels
	);
	if (data == nullptr) {
		std::cout << "ERROR load_image failed: " << filename << "\n";
		throw std::runtime_error("FileNotFound");
	}
	if (channels == 0) {
		image.channels = channels_in_file;
	}
	else {
		image.channels = channels;
	}
	std::size_t data_size = image.channels * image.width * image.height;
	image.data.resize(data_size);
	memcpy(image.data.data(), data, data_size);
	stbi_image_free(data);
	return image;
}

Image load_image(std::string filename)
{
	return load_image_channels(filename, 0);
}

Image load_image_force_channels(std::string filename, int force_channels) {
	Image image = load_image_channels(filename, force_channels);
	image.channels = force_channels;
	return image;
}

Image create_color_image(int width, int height, int channels, uint32_t pixel) {
	// pixel is 0xAABBGGRR
	Image image;
	image.width = width;
	image.height = height;
	image.channels = channels;
	std::size_t pixel_count = width * height;
	image.data.resize(pixel_count * channels);
	for (std::size_t ipixel = 0; ipixel < pixel_count; ++ipixel) {
		uint8_t* p = &image.data[ipixel * channels];
		for (int icomp = 0; icomp < channels; ++icomp) {
			// data should be R, G, B, A
			p[icomp] = (pixel >> ((icomp) * 8)) & 0xff;
		}
	}
	return image;
}

} // namespace imageutil
