import math

import vmath
from vmathlib import vcolor, vutil

import drawutil

from unit_manager import Unit


class Testease(Unit):
	def tick(self, delta_time):
		self._draw_ease_panel(vmath.Vector3())

	def _draw_ease_panel(self, panel_position):
		exp = 3
		ease_funcs = [
			vutil.alpha_ease_none,
			vutil.alpha_ease_in,
			vutil.alpha_ease_out,
			vutil.alpha_ease_in_out
		]

		game_time = self.world.game_time
		time_scale = 0.5
		alpha = vutil.fract(game_time * time_scale)
		for index, ease_func in enumerate(ease_funcs):
			eased_alpha = ease_func(alpha, exp)
			circle_position = panel_position + vmath.Vector3(-2.0, 0.0, 0.0) * index
			self._draw_ease_circle(circle_position, 0.8, eased_alpha)
			line_position = circle_position + vmath.Vector3(0.0, 0.0, -1.5)
			self._draw_ease_line(line_position, 1.6, eased_alpha)

	def _draw_ease_circle(self, position, radius, alpha):
		transform = vmath.Transform()
		transform.translation = position
		drawutil.draw_circle(transform, radius)
		theta = vutil.lerp(0.0, math.pi * 2.0, alpha)
		delta_position = vmath.Vector3(math.sin(theta), 0.0, math.cos(theta)) * radius
		drawutil.draw_cube(position + delta_position, 0.1, color=vcolor.BLUE)

	def _draw_ease_line(self, position, length, alpha):
		delta_half = vmath.Vector3(length, 0.0, 0.0) * 0.5
		start = position + delta_half
		end = position - delta_half
		drawutil.draw_line(start, end)
		res_position = vutil.lerp(start, end, alpha)
		drawutil.draw_cube(res_position, 0.1, color=vcolor.BLUE)
