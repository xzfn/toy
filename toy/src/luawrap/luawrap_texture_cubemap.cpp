#include "luawrap_texture_cubemap.h"

#include "texture_cubemap.h"
#include "load_helper.h"


void luawrap_texture_cubemap(sol::table& m) {
	auto t = m.new_usertype<TextureCubemap>("TextureCubemap");

	t.set_function("create", sol::factories(&create_texture_cubemap));
}
