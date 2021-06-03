import toy
import vmath

import drawutil
from unit_manager import Unit

from units.bullet import Bullet


class Player(Unit):
	def __init__(self, world, unit_id, param):
		super().__init__(world, unit_id, param)

		self.transform = vmath.Transform()
		self._fire_cd = 0.2
		self._fire_countdown = self._fire_cd
		
	def tick(self, delta_time):
		if toy.get_input_manager().get_key(ord('F')):
			old_translation = self.transform.translation
			delta_movement = delta_time * vmath.Vector3(0.0, 0.0, 1.0)
			self.transform.translation = old_translation + delta_movement

		self._fire_countdown -= delta_time
		if toy.get_input_manager().get_key_down(ord('K')):
			if self._fire_countdown <= 0.0:
				self._fire_countdown = self._fire_cd
				param = {
					'start_pos': self.transform.translation,
					'direction': vmath.Vector3(0.0, 0.7, 0.7),
					'speed': 10.0
				}
				print('fire bullet')
				self.world.unit_manager.create_unit(Bullet, param)


	def render(self):
		drawutil.draw_transform(self.transform)
