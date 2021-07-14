
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

        light_transform = self.light_transform
        self.light = toy.Light()
        self.light.set_type(toy.LightType.Spot)
        self.light.set_color(vcolor.GREEN)
        self.light.set_spot_inner_angle(math.radians(30))
        self.light.set_spot_outer_angle(math.radians(40))
        self.light.set_direction(light_transform.rotation.to_matrix3()[2])
        toy.app.light_manager.add_light(self.light)

    def tick(self, delta_time):
        light_transform = self.light_transform
        drawutil.draw_transform(light_transform)
        direction = light_transform.rotation.to_matrix3()[2]
        self.light.set_position(light_transform.translation)
        self.light.set_direction(direction)

        if toy.app.input_manager.get_key_down(keycodes.VK_K) or True:
            camera_controller = toy.app.camera_manager.get_camera_controller()
            transform = camera_controller.get_transform()
            matrix3 = transform.rotation.to_matrix3()
            print(transform.rotation)
            print(matrix3)
            matrix3[0] *= -1.0
            matrix3[2] *= -1.0
            transform.rotation = vmath.Quaternion.from_matrix3(matrix3)
            print(transform.rotation)
            self.light_transform = transform
