
import math

import vmath
from vmathlib import vcolor, vutil
import vmathlib

import toy
import keycodes

import drawutil
from unit_manager import Unit


class Cameraman(Unit):
    
    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)

        self.camera_transform = vmath.Transform()

    def tick(self, delta_time):
        camera = toy.app.camera_manager.get_camera()

        camera_transform = self.camera_transform
        if camera.is_perspective():
            perspective_data = camera.get_perspective_data()
            drawutil.draw_perspective(
                camera_transform,
                perspective_data.fov,
                perspective_data.aspect,
                1.0,
                10.0
            )
        else:
            orthographic_data = camera.get_orthographic_data()
            drawutil.draw_orthographic(
                camera_transform,
                orthographic_data.left * 0.1,
                orthographic_data.right * 0.1,
                orthographic_data.bottom * 0.1,
                orthographic_data.top * 0.1,
                orthographic_data.z_near * 0.1,
                orthographic_data.z_far * 0.1,
            )

        if toy.app.input_manager.get_key_down(keycodes.VK_H):
            camera_controller = toy.app.camera_manager.get_camera_controller()
            camera_controller.set_transform(camera_transform)

        if toy.app.input_manager.get_key_down(keycodes.VK_J):
            camera_controller = toy.app.camera_manager.get_camera_controller()
            self.camera_transform = camera_controller.get_transform()

    def render(self):
        pass
