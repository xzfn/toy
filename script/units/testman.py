
import math

import toy

import vmath
from vmathlib import vcolor, vutil, vcolors
import vmathlib

import drawutil
from unit_manager import Unit

import toyentry


class Testman(Unit):
    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)
        self.mesh = toy.Mesh.create(toy.MeshData.create_unit_quad())
        self.cube_mesh = toy.Mesh.create(toy.MeshData.create_unit_cube())
        self.material = toyentry.app.material
        self.rgb_materials = toyentry.app.rgb_materials
        self.spot_light = None
        self.point_light = None
        self.init_lights()

    def init_lights(self):
        self.spot_light = toy.Light()
        self.spot_light.set_type(toy.LightType.Spot)
        self.spot_light.set_color(vcolor.WHITE * 0.5)
        self.spot_light.set_spot_inner_angle(math.radians(30))
        self.spot_light.set_spot_outer_angle(math.radians(40))
        toy.app.light_manager.add_light(self.spot_light)

        self.point_light = toy.Light()
        self.point_light.set_type(toy.LightType.Point)
        self.point_light.set_color(vcolor.WHITE * 0.2)
        transform = vmath.Transform()
        transform.translation = vmath.Vector3(5.0, 5.0, 5.0)
        self.point_light.set_transform(transform)
        toy.app.light_manager.add_light(self.point_light)

    def tick(self, delta_time):
        self.draw_ground()
        self.draw_test_scene()
        self.draw_lights()

    def draw_ground(self):
        ground_transform = vmath.Transform()
        ground_transform.translation = vmath.Vector3(0.0, -0.01, 0.0)
        ground_transform.scale = vmath.Vector3(20.0, 20.0, 20.0) * 50
        ground_transform.rotation = vmath.Quaternion.from_angle_axis(-math.pi / 2.0, vutil.VEC3_X)
        toy.app.render_manager.add_mesh(self.mesh, ground_transform.to_matrix4(), self.material)

    def draw_test_scene(self):
        transform = vmath.Transform()
        for i in range(3):
            for j in range(3):
                transform.translation = vmath.Vector3(i * 3, 2.0, j * 3)
                angle = (i * 3 + j) * self.world.game_time
                rotation = vmath.Quaternion.from_euler_angles(vmath.Vector3(0.0, angle, angle * 0.3))
                transform.rotation = rotation
                toy.app.render_manager.add_mesh(self.cube_mesh, transform.to_matrix4(), self.material)

        transform = vmath.Transform()
        for i in range(3):
            transform.translation = vmath.Vector3(-5.0, i * 2.0, -5.0)
            material = self.rgb_materials[i]
            toy.app.render_manager.add_mesh(self.cube_mesh, transform.to_matrix4(), material)

        input_manager = toy.app.input_manager
        if input_manager.get_joystick_button(0):
            transform = vmath.Transform()
            thumb = input_manager.get_joystick_thumb_right()
            diff = vmath.Vector3(thumb[0], 0.0, thumb[1])
            transform.translation = vmath.Vector3(0.0, 5.0, 0.0) + diff * 3.0
            trigger_left = input_manager.get_joystick_trigger_left()
            trigger_right = input_manager.get_joystick_trigger_right()
            transform.rotation = vmath.Quaternion.from_euler_angles(vmath.Vector3(trigger_left, trigger_right, 0.0))
            toy.app.render_manager.add_mesh(self.cube_mesh, transform.to_matrix4(), self.material)

    def draw_lights(self):
        sun_direction = vmath.Vector3(math.cos(self.world.game_time), -1.0, math.sin(self.world.game_time)).normalize()
        transform = vmath.Transform()
        transform.rotation = vmath.Quaternion.from_look_rotation(sun_direction, vutil.VEC3_Y)
        toy.app.light_manager.get_sun().set_transform(transform)
        transform.scale = vmath.Vector3(10.0, 10.0, 10.0)
        sun_origin = vmath.Vector3(0.0, 5.0, 0.0)
        drawutil.draw_direction(sun_origin, sun_direction)

        if self.spot_light:
            t = math.modf(self.world.game_time * 0.1)[0]
            self.spot_light.set_color(vcolor.bounce(vcolors.red, vcolors.lime, t))
            spot_position = vmath.Vector3(0.0, 10.0, -4.0 * vutil.ping_pong(t))
            transform = vmath.Transform()
            transform.translation = spot_position
            transform.rotation = vmath.Quaternion.from_euler_angles(vmath.Vector3(-vutil.ping_pong(t) - math.pi / 2.0, 0.0, 0.0))
            self.spot_light.set_transform(transform)
            drawutil.draw_direction(spot_position, -transform.rotation.to_matrix3()[2])
