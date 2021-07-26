

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
		self.material = toyentry.app.material
		self.meshes = []
		primitive_objs = gltfutil.GLTFLoader('../../toy/resource/gltf/shiba/scene.gltf').build()
		for mesh_data in primitive_objs:
			mesh = toy.Mesh.create(mesh_data)
			self.meshes.append(mesh)

	def tick(self, delta_time):
		for mesh in self.meshes:
			toy.app.render_manager.add_mesh(mesh, self.transform.to_matrix4(), self.material)

		if toy.app.input_manager.get_key_down(keycodes.VK_R):
			self.transform = toy.app.camera_manager.get_camera_controller().get_transform()
