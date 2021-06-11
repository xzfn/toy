import toy
import vmath

import drawutil
from unit_manager import Unit


class Bullet(Unit):
	def __init__(self, world, unit_id, param):
		super().__init__(world, unit_id, param)

		self.start_pos = param['start_pos']
		self.transform = vmath.Transform()
		self.transform.translation = self.start_pos

		self.direction = param['direction']
		self.speed = param['speed']
		self.duration = param.get('duration', 3.0)
		toy.app.timer_manager.add_timer(self.duration, self.destroy)

	def tick(self, delta_time):
		velocity = self.direction * self.speed
		new_pos = self.transform.translation + velocity * delta_time
		self.transform.translation = new_pos

	def render(self):
		drawutil.draw_transform(self.transform)
