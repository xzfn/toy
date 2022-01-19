#include "luawrap_text_builder.h"

#include "text_builder.h"


void luawrap_text_builder(sol::table& m) {
	auto t = m.new_usertype<TimedTextBuilder>("TimedTextBuilder");

	t.set_function("add_text", &TimedTextBuilder::add_text);
	t.set_function("add_screen_text", &TimedTextBuilder::add_screen_text);
}
