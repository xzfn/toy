

import math

import vmath
from vmathlib import vcolor, vutil
import vmathlib

import toy
import keycodes

import drawutil
from unit_manager import Unit

import mathutil
import gltfutil
import toyentry


class Testgltf(Unit):
	def __init__(self, world, unit_id, param):
		super().__init__(world, unit_id, param)
		self.transform = vmath.Transform()
		primitive_objs, images = gltfutil.GLTFLoader('../../toy/resource/gltf/shiba/scene.gltf').build()
		self.primitive_objs = primitive_objs
		self.images = images

	def tick(self, delta_time):
		for primitive_obj in self.primitive_objs:
			mesh, material = primitive_obj
			toy.app.render_manager.add_mesh(mesh, self.transform.to_matrix4(), material)

		if toy.app.input_manager.get_key_down(keycodes.VK_R):
			self.transform = toy.app.camera_manager.get_camera_controller().get_transform()
