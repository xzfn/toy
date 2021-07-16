
import math

import vmath
from vmathlib import vcolor, vutil
import vmathlib

import toy
import keycodes

import drawutil
from unit_manager import Unit

import mathutil


class Shadowman(Unit):

    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)
        self.camera_transform = vmath.Transform()
        self.perspective_data = None

    def tick(self, delta_time):
        if toy.app.input_manager.get_key_down(keycodes.VK_T):
            camera_controller = toy.app.camera_manager.get_camera_controller()
            self.camera_transform = camera_controller.get_transform()
            camera = toy.app.camera_manager.get_camera()
            self.perspective_data = camera.get_perspective_data()

        if not self.perspective_data:
            return
        camera_transform = self.camera_transform
        perspective_data = self.perspective_data

        fov = perspective_data.fov
        aspect = perspective_data.aspect
        z_near = perspective_data.z_near
        z_far = perspective_data.z_far

        percents = [0.0, 0.01, 0.05, 1.0]
        splits = [z_near + (z_far - z_near) * percent for percent in percents]

        sun = toy.app.light_manager.get_sun()
        sun_orientation = sun.get_transform().rotation

        for isplit in range(len(splits) - 1):
            split_near = splits[isplit]
            split_far = splits[isplit + 1]
            ortho_transform, ortho_param = mathutil.perspective_to_bounding_orthographic(
                camera_transform, fov, aspect, split_near, split_far, sun_orientation
            )
            drawutil.draw_perspective(camera_transform, fov, aspect, split_near, split_far)
            drawutil.draw_orthographic(ortho_transform, *ortho_param, color=vcolor.hue(isplit * 0.3))
            drawutil.draw_transform(ortho_transform, 1.0)
