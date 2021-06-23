
import math

import toy

import vmath
from vmathlib import vcolor, vutil

import drawutil
from unit_manager import Unit


class Enemy(Unit):
	def __init__(self, world, unit_id, param):
		super().__init__(world, unit_id, param)
		self.transform = vmath.Transform()
		self.color = vcolor.hue(0.0)

		self.transform.translation = vmath.Vector3(0.0, 0.0, 10.0)

		builder = toy.GeometryBuilder()
		builder.add_cube(vmath.Vector3(), 1.0, self.color)
		self.geometry_mesh = toy.GeometryMesh.create(builder.build_data())

	def tick(self, delta_time):
		alpha = math.modf(self.world.game_time * 0.2)[0]
		theta = alpha * math.pi * 2.0
		radius = 5.0
		self.transform.translation = vmath.Vector3(math.cos(theta), 0.0, math.sin(theta)) * radius
		self.transform.rotation = vmath.Quaternion.from_angle_axis(theta, vutil.VEC3_Y)
		self.color = vcolor.hue(alpha)

	def render(self):
		position = self.transform.translation
		drawutil.draw_line(position, position + vutil.VEC3_Y, self.color)
		toy.app.render_manager.add_geometry_mesh(self.geometry_mesh, self.transform.to_matrix4(), toy.app.material_lines)
