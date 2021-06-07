#include "ascii_texture.h"


static std::vector<CharUv> calc_char_uvs();

std::vector<CharUv> g_char_uvs = calc_char_uvs();


CharUv get_char_uv(char c) {
	if (c < 0x20 || c > 0x7e) {
		c = 0x7f;
	}
	char index = c - (char)0x20;
	return g_char_uvs[index];
}


std::vector<CharUv> calc_char_uvs() {
	int extent = 512;
	int char_width = ASCIITextureInfo::char_width;
	int char_height = ASCIITextureInfo::char_height;
	int col_gap = 40;
	int row_gap = 60;
	int ncols = 12;
	int nrows = 8;
	int anchor_x = 17;
	int anchor_y = 73;
	char char_start = 32;
	char char_stop = 0x7e + 1;

	float du = 1.0f * char_width / extent;
	float dv = 1.0f * char_height / extent;

	std::vector<CharUv> char_uvs;

	for (char c = char_start; (unsigned char)c <= (unsigned char)char_stop; ++c) {
		char i = c - char_start;
		int row = i / ncols;
		int col = i % ncols;
		int x = anchor_x + col_gap * col;
		int y = anchor_y + row_gap * row - char_height;

		float u1 = 1.0f * x / extent;
		float v1 = 1.0f * y / extent;
		float u2 = u1 + du;
		float v2 = v1 + dv;

		CharUv char_uv{
			u1, v1, u2, v2
		};
		char_uvs.push_back(char_uv);
	}

	return char_uvs;
}
