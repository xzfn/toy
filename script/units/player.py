from vmathlib import vutil
import toy
import vmath

import toyentry
import keycodes

import drawutil
from unit_manager import Unit

from units.bullet import Bullet


class Player(Unit):
	def __init__(self, world, unit_id, param):
		super().__init__(world, unit_id, param)

		self.transform = vmath.Transform()
		self.speed = 3.0
		self.mesh = toy.Mesh.create(toy.MeshData.create_unit_cube())
		self.material = toyentry.app.material
		self._fire_cd = 0.2
		self._fire_countdown = self._fire_cd

	def tick(self, delta_time):
		if toy.app.input_manager.get_key(ord('F')):
			old_translation = self.transform.translation
			delta_movement = delta_time * vmath.Vector3(0.0, 0.0, 1.0)
			self.transform.translation = old_translation + delta_movement

		self._fire_countdown -= delta_time
		if toy.app.input_manager.get_key_down(ord('K')):
			if self._fire_countdown <= 0.0:
				self._fire_countdown = self._fire_cd
				param = {
					'start_pos': self.transform.translation,
					'direction': vmath.Vector3(0.0, 0.7, 0.7),
					'speed': 10.0
				}
				print('fire bullet')
				self.world.unit_manager.create_unit(Bullet, param)

		axis_x = 0.0
		axis_y = 0.0
		if toy.app.input_manager.get_key(keycodes.VK_UP):
			axis_y += 1.0
		if toy.app.input_manager.get_key(keycodes.VK_DOWN):
			axis_y += -1.0
		if toy.app.input_manager.get_key(keycodes.VK_LEFT):
			axis_x += 1.0
		if toy.app.input_manager.get_key(keycodes.VK_RIGHT):
			axis_x += -1.0
		movement_input = vmath.Vector3(axis_x, 0.0, axis_y)
		turn_input = 0.0
		if toy.app.input_manager.get_key(keycodes.VK_NUMPAD0):
			turn_input += 1.0

		if movement_input.length() > 1.0:
			movement_input.normalize_self()
		if movement_input.length() > 0.0:
			self.transform.translation += movement_input * delta_time * self.speed
		if turn_input != 0.0:
			rotate = vmath.Quaternion.from_angle_axis(turn_input * delta_time, vutil.VEC3_Y)
			self.transform.rotation *= rotate

	def render(self):
		drawutil.draw_transform(self.transform)
		toy.app.render_manager.add_mesh(self.mesh, self.transform.to_matrix4(), self.material)
