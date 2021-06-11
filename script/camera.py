import toy
import vmath


class Camera:
	def __init__(self):
		pass

	def tick(self, delta_time):
		input_manager = toy.app.input_manager
		if input_manager.get_key_down(ord('P')):
			self.toggle_freeview()

		camera_manager = toy.app.camera_manager
		if not camera_manager.is_controlled():
			camera = camera_manager.get_camera()

			camera.set_view

	def toggle_freeview(self):
		camera_manager = toy.app.camera_manager
		camera_manager.set_is_controlled(not camera_manager.is_controlled())
		