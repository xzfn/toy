#include "luawrap_timer_manager.h"

#include "timer_manager.h"


void luawrap_timer_manager(sol::table& m) {
	auto t = m.new_usertype<TimerManager>("TimerManager");
	t[sol::call_constructor] = sol::constructors<TimerManager()>();
	t.set_function("schedule", &TimerManager::schedule);
	t.set_function("add_timer_unsafe", &TimerManager::add_timer);
	t.set_function("add_timer", [](TimerManager* this_, double duration, sol::protected_function lua_callback) {
		TimerCallback callback = [lua_callback]() {
			auto lua_ret = lua_callback();
			if (!lua_ret.valid()) {
				sol::error err = lua_ret;
				std::cout << "ERROR lua error " << err.what() << "\n";
			}
		};
		return this_->add_timer(duration, callback);
	});

	t.set_function("add_repeat_timer_unsafe", &TimerManager::add_repeat_timer);
	t.set_function("add_repeat_timer", [](TimerManager* this_, double duraiton, sol::protected_function lua_callback) {
		TimerCallback callback = [lua_callback]() {
			auto lua_ret = lua_callback();
			if (!lua_ret.valid()) {
				sol::error err = lua_ret;
				std::cout << "ERROR lua error " << err.what() << "\n";
			}
		};
		return this_->add_repeat_timer(duraiton, callback);
	});

	t.set_function("cancel_timer", &TimerManager::cancel_timer);
	t.set_function("clear_timers", &TimerManager::clear_timers);
}
