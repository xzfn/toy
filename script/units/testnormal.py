
import math

import toy

import vmath
from vmathlib import vcolor, vutil, vcolors
import vmathlib

import drawutil
from unit_manager import Unit

import toyentry


class Testnormal(Unit):
	def __init__(self, world, unit_id, param):
		super().__init__(world, unit_id, param)

	def tick(self, delta_time):

		z_vec = vmath.Vector3(0.0, 0.0, 1.0)
		tripoints = [
			vmath.Vector3(-0.5, 0.2, 0.0),
			vmath.Vector3(1.0, 0.0, 0.0),
			vmath.Vector3(1.0, 1.0, 0.0)
		]

		transform = vmath.Transform()
		transform.translation = vmath.Vector3(-5.0, 0.0, 0.0)
		game_time = self.world.game_time
		t = vutil.fract(game_time * 0.2)
		delta_x = vutil.ping_pong(t) * 3.0 - 0.5
		transform.scale = vmath.Vector3(1.0 + delta_x, 1.0, 1.0)

		transform_inverse = transform.to_matrix4().inverse()
		transform_inverse_transpose = transform_inverse.transpose()

		transformed_normals_bad = []
		transformed_normals_good = []

		for i in range(3):
			p1 = tripoints[i]
			p2 = tripoints[(i+1)%3]
			drawutil.draw_line(p1, p2)
			mid = (p1 + p2) * 0.5
			vec = p2 - p1
			n = vec.cross(z_vec)
			n.normalize_self()
			drawutil.draw_ray_xy(mid, n, color=vcolor.BLUE)
			# transform normal
			transformed_normals_bad.append(transform.transform_vector(n).normalize())
			transformed_normals_good.append(transform_inverse_transpose.transform_vector(n).normalize())

		transformed_tripoints = [
			transform.transform_point(p) for p in tripoints
		]

		for i in range(3):
			p1 = transformed_tripoints[i]
			p2 = transformed_tripoints[(i+1)%3]
			drawutil.draw_line(p1, p2)
			mid = (p1 + p2) * 0.5
			vec = p2 - p1
			
			drawutil.draw_ray_xy(mid, transformed_normals_bad[i], color=vcolor.GRAY)
			drawutil.draw_ray_xy(mid, transformed_normals_good[i], color=vcolor.GREEN)
