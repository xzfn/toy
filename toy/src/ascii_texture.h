#pragma once

#include <vector>


struct ASCIITextureInfo {
	static constexpr int char_width = 30;
	static constexpr int char_height = 52;
};

struct CharUv {
	float u1;
	float v1;
	float u2;
	float v2;
};

CharUv get_char_uv(char c);
