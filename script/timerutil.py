import toy
import vmath


class RepeatUntil:
	def __init__(self, func):
		self.func = func
		self.handler = None

	def set_handler(self, handler):
		self.handler = handler

	def __call__(self):
		should_continue = self.func()
		if not should_continue:
			self.handler()

def add_repeat_util(duration, func):
	repeat_until = RepeatUntil(func)
	timer = toy.app.timer_manager.add_repeat_timer(duration, repeat_until)
	def canceler(timer=timer):
		toy.app.timer_manager.cancel_timer(timer)
	repeat_until.set_handler(canceler)
	return timer


class DynamicBulletText:
	def __init__(self, position, text, duration):
		self.text = text
		self.position = position
		self.duration = duration
		self.countdown = duration

		self.timer = add_repeat_util(0.0, self.tick)

	def tick(self):
		toy.app.timed_text_builder.add_text(self.position, self.text, vmath.Vector3(), 0.0)
		delta_time = toy.app.delta_time
		self.countdown -= delta_time
		if self.countdown < 0:
			return False
		return True

	def reset_duration(self):
		self.countdown = self.duration

	def update_text(self, text):
		self.text = text
		self.reset_duration()

	def cancel(self):
		toy.app.timer_manager.cancel_timer(self.timer)
