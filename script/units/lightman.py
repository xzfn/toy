
import math

import vmath
from vmathlib import vcolor, vutil
import vmathlib

import toy
import keycodes

import drawutil
from unit_manager import Unit


class Lightman(Unit):

    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)

        self.light_transform = vmath.Transform()

        self.light = toy.Light()
        self.light.set_type(toy.LightType.Spot)
        self.light.set_color(vcolor.GREEN)
        self.light.set_spot_inner_angle(math.radians(30))
        self.light.set_spot_outer_angle(math.radians(40))
        toy.app.light_manager.add_light(self.light)

    def tick(self, delta_time):
        light_transform = self.light_transform
        drawutil.draw_transform(light_transform)
        self.light.set_transform(light_transform)

        if True or toy.app.input_manager.get_key_down(keycodes.VK_K):
            camera_controller = toy.app.camera_manager.get_camera_controller()
            transform = camera_controller.get_transform()
            transform.translation += vmath.Vector3(0.0, -2.0, 0.0)
            self.light_transform = transform
