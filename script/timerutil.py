import toy
import vmath


def add_repeat_while(duration, func):
	def wrapper():
		should_continue = func()
		if not should_continue:
			toy.app.timer_manager.cancel_timer(timer)
	timer = toy.app.timer_manager.add_repeat_timer(duration, wrapper)
	return timer

class DynamicBulletText:
	def __init__(self, position, text, duration):
		self.text = text
		self.position = position
		self.duration = duration
		self.countdown = duration

		self.timer = add_repeat_while(0.0, self.tick)

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
