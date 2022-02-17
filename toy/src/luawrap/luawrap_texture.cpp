#include "luawrap_texture.h"

#include "texture.h"
#include "load_helper.h"


void luawrap_texture(sol::table& m) {
	auto t = m.new_usertype<Texture>("Texture");

	t.set_function("create", sol::factories(&create_texture));
	t.set_function("create_color", sol::factories(&create_color_texture));
	t.set_function("create_rgba32", sol::factories(&create_rgba32_texture));

}
