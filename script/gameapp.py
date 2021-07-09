
import os
import time
import math
import random
import sys

import numpy as np

import toy
import vmath
from vmathlib import vcolors, vutil, vcolor

import remoteconsole.server
import retroreload

#import toyqt
from world import World
import drawutil
import shaderutil
import mathutil

from units.player import Player
from units.enemy import Enemy
import keycodes


def build_quad_mesh_data():
    mesh_data = toy.MeshData()
    vertices = [
        vmath.Vector3(0.5, 0.5, 0.0),
        vmath.Vector3(0.5, -0.5, 0.0),
        vmath.Vector3(-0.5, 0.5, 0.0),
        vmath.Vector3(-0.5, -0.5, 0.0),
    ]
    normals = [
        vmath.Vector3(0.0, 0.0, -1.0),
        vmath.Vector3(0.0, 0.0, -1.0),
        vmath.Vector3(0.0, 0.0, -1.0),
        vmath.Vector3(0.0, 0.0, -1.0),
    ]
    uvs = [
        vmath.Vector2(0.0, 0.0),
        vmath.Vector2(0.0, 1.0),
        vmath.Vector2(1.0, 0.0),
        vmath.Vector2(1.0, 1.0),
    ]
    indices = [
        0, 1, 2, 2, 1, 3
    ]
    mesh_data.set_vertices_data(vertices, normals, uvs)
    mesh_data.set_indices(indices)
    return mesh_data

class App:
    def __init__(self):
        server_address = ('127.0.0.1', 9000)
        self.console_environment = {}
        self.console_server = remoteconsole.server.ConsoleServer(server_address)
        console_handler = remoteconsole.server.PythonCodeHandler(self.console_environment)
        self.console_server.set_request_handler(console_handler)
        #toy.set_thread_priority(self.console_server.http_server_thread.native_id, 1)
        self.world = None

    def startup(self):
        toy.app.bind_key_down(self.on_key_down)
        #toyqt.startup()
        #toyqt.qtwindow.on_shoot = self.on_shoot
        self.world = World()

        self.texture = toy.Texture.create('resource/cube.png')
        self.pipeline = toy.BasicPipeline.create('shader/basic.vert.spv', 'shader/basic.frag.spv')
        self.material = toy.Material.create(self.pipeline, self.texture)
        mesh_data = build_quad_mesh_data()
        self.mesh = toy.Mesh.create(mesh_data)

        self.cube_mesh = toy.Mesh.create(toy.MeshData.create_unit_cube())

        builder = toy.GeometryBuilder()
        builder.add_cube(vmath.Vector3(), 3.0, vcolors.cyan)
        builder.add_cube(vmath.Vector3(1.0, 0.0, 0.0), 0.5, vcolors.magenta)
        geometry_mesh_data = builder.build_data()
        self.geometry_mesh = toy.GeometryMesh.create(geometry_mesh_data)

        self.light = toy.Light()
        self.light.set_type(toy.LightType.Point)
        self.light.set_position(vmath.Vector3(5.0, 5.0, -5.0))
        toy.app.light_manager.add_light(self.light)

        self.spot_light = toy.Light()
        self.spot_light.set_type(toy.LightType.Spot)
        self.spot_light.set_color(vcolor.WHITE * 0.5)
        self.spot_light.set_position(vmath.Vector3(0.0, 0.0, 0.0))
        self.spot_light.set_spot_inner_angle(math.radians(30))
        self.spot_light.set_spot_outer_angle(math.radians(40))
        self.spot_light.set_direction(vmath.Vector3(0.0, -1.0, 0.5).normalize())
        toy.app.light_manager.add_light(self.spot_light)

        self.spot_light2 = toy.Light()
        self.spot_light2.set_type(toy.LightType.Spot)
        self.spot_light2.set_color(vcolor.WHITE * 0.5)
        self.spot_light2.set_position(vmath.Vector3(0.0, 5.0, 5.0))
        self.spot_light2.set_spot_inner_angle(math.radians(30))
        self.spot_light2.set_spot_outer_angle(math.radians(40))
        self.spot_light2.set_direction(vmath.Vector3(0.0, -1.0, 0.1).normalize())
        toy.app.light_manager.add_light(self.spot_light2)

        self.world.unit_manager.create_unit(Player)
        self.world.unit_manager.create_unit(Enemy)

        self.red_texture = toy.Texture.create_color(vcolor.RED)
        self.green_texture = toy.Texture.create_color(vcolor.GREEN)
        self.blue_texture = toy.Texture.create_color(vcolor.BLUE)
        self.red_material = toy.Material.create(self.pipeline, self.red_texture)
        self.green_material = toy.Material.create(self.pipeline, self.green_texture)
        self.blue_material = toy.Material.create(self.pipeline, self.blue_texture)
        self.rgb_materials = [self.red_material, self.green_material, self.blue_material]

    def on_shoot(self):
        drawutil.draw_line(vmath.Vector3(0.0, 0.0, 0.0), vmath.Vector3(10.0, 10.0, 10.0), vmath.Vector3(1.0, 1.0, 0.0), 5.0)

    def on_key_down(self, key):
        if key == keycodes.VK_F5:
            if toy.app.input_manager.get_key(keycodes.VK_CONTROL):
                print('reload shader')
                shaderutil.reload_pipelines([self.pipeline])
            else:
                print('reload script')
                main_script_folder = os.path.dirname(sys.modules[__name__].__file__)
                retroreload.retroreload_script_folders([main_script_folder])

    def update(self):
        self.console_server.service()
        time.sleep(0.001)
        #toyqt.update()
        game_time = self.world.game_time
        mouse_dx, mouse_dy = toy.app.input_manager.get_mouse_move()
        mouse_wheel = toy.app.input_manager.get_mouse_wheel()

        delta_time = toy.app.delta_time
        drawutil.draw_screen_text(vmath.Vector3(0, 20, 0), 'dt {:6.4f}'.format(delta_time), color=vcolors.white)
        self.world.tick(delta_time)

        drawutil.draw_ground_grid()
        length = 10.0
        drawutil.draw_text(vutil.VEC3_X * length, 'X', color=vcolors.red)
        drawutil.draw_text(vutil.VEC3_Z * length, 'Z', color=vcolors.blue)
        drawutil.draw_text(-vutil.VEC3_X * length, '-X', color=vcolors.red)
        drawutil.draw_text(-vutil.VEC3_Z * length, '-Z', color=vcolors.blue)

        transform = vmath.Transform()
        transform.translation = vmath.Vector3(0.0, 0.0, -10.0)
        drawutil.draw_transform(transform)

        mouse_x, mouse_y = toy.app.input_manager.get_mouse_position()

        self.world.render()

        ground_transform = vmath.Transform()
        ground_transform.translation = vmath.Vector3(0.0, -0.01, 0.0)
        ground_transform.scale = vmath.Vector3(20.0, 20.0, 20.0)
        ground_transform.rotation = vmath.Quaternion.from_angle_axis(math.pi / 2.0, vutil.VEC3_X)
        toy.app.render_manager.add_mesh(self.mesh, ground_transform.to_matrix4(), self.material)

        percent = math.modf(mouse_x / 300.0)[0]
        self.material.set_base_color(vmath.Vector3(1.0, percent, 1.0))
        transform.translation = vmath.Vector3(5.0, 5.0, 0.0)
        matrix = transform.to_matrix4()
        drawutil.draw_transform(transform)
        toy.app.render_manager.add_mesh(self.mesh, matrix, self.material)
        
        transform.translation = vmath.Vector3(5.0, 10.0, 0.0)
        transform.scale = vmath.Vector3(2.0, 2.0, 2.0)
        toy.app.render_manager.add_mesh(self.mesh, transform.to_matrix4(), self.material)
        
        transform.translation = vmath.Vector3(5.0, 15.0, 0.0)
        transform.rotation = vmath.Quaternion.from_angle_axis(mouse_y / 10.0, vmath.Vector3(0.0, 1.0, 0.0))
        toy.app.render_manager.add_mesh(self.mesh, transform.to_matrix4(), self.material)


        transform = vmath.Transform()
        for i in range(3):
            for j in range(3):
                transform.translation = vmath.Vector3(i * 3, 2.0, j * 3)
                angle = (i * 3 + j) * self.world.game_time
                rotation = vmath.Quaternion.from_euler_angles(vmath.Vector3(0.0, angle, angle * 0.3))
                transform.rotation = rotation
                toy.app.render_manager.add_mesh(self.cube_mesh, transform.to_matrix4(), self.material)

        if False:
            transform.translation = vmath.Vector3(0.0, 3.0, 0.0)
            transform.rotation = vmath.Quaternion.from_angle_axis(self.world.game_time, vmath.Vector3(0.0, 0.0, 1.0))
            toy.app.render_manager.add_geometry_mesh(self.geometry_mesh, transform.to_matrix4(), toy.app.material_lines)
            transform.translation = vmath.Vector3(0.0, 10.0, 0.0)
            transform.scale = vmath.Vector3(1.0, 1.0, 1.0) * math.modf(self.world.game_time)[0] + vmath.Vector3(1, 1, 1)
            toy.app.render_manager.add_geometry_mesh(self.geometry_mesh, transform.to_matrix4(), toy.app.material_lines)

        transform = vmath.Transform()
        for i in range(3):
            transform.translation = vmath.Vector3(-5.0, i * 2.0, -5.0)
            material = self.rgb_materials[i]
            toy.app.render_manager.add_mesh(self.cube_mesh, transform.to_matrix4(), material)

        #drawutil.draw_sphere(self.light.get_position())
        alpha = math.modf(self.world.game_time * 0.4)[0]
        c = vcolor.bounce(vcolors.red, vcolors.lime, alpha)
        self.light.set_color(vcolor.bounce(vcolors.red, vcolors.lime, alpha))
        
        sun_direction = vmath.Vector3(math.cos(self.world.game_time), -1.0, math.sin(self.world.game_time)).normalize()
        toy.app.light_manager.get_sun().set_direction(sun_direction)
        sun_origin = vmath.Vector3(0.0, 5.0, 0.0)
        drawutil.draw_line(sun_origin, sun_origin + sun_direction)
        transform = vmath.Transform()
        transform.translation = sun_origin + vmath.Vector3(0.0, sun_direction.y, 0.0)
        radius = vmath.Vector3(sun_direction.x, 0.0, sun_direction.z).length()
        transform.scale = vmath.Vector3(radius, radius, radius)
        drawutil.draw_circle(transform, color=vcolor.BLUE)
        drawutil.draw_line(sun_origin, transform.translation, color=vcolor.GREEN)

        t = math.modf(self.world.game_time * 0.1)[0]
        #t = vutil.ping_pong(t)
        drawutil.draw_split_line(vmath.Vector3(-10, 1, -10), vmath.Vector3(-10, 1, 10), vcolor.GRAY, vcolor.RED, t)
        drawutil.draw_travel_line(vmath.Vector3(-10, 2, -10), vmath.Vector3(-10, 2, 10), vcolor.GRAY, vcolor.RED, t)
        drawutil.draw_travel_line(vmath.Vector3(-10, 3, -10), vmath.Vector3(-10, 3, 10), vcolor.GRAY, vcolor.GREEN, t)

        spot_direction = vmath.Vector3(0.0, -1.0, 0.8 * vutil.ping_pong(t)).normalize()
        self.spot_light.set_direction(spot_direction)
        spot_position = vmath.Vector3(0.0, 10.0, -4.0 * vutil.ping_pong(t))
        self.spot_light.set_position(spot_position)
        self.spot_light2.set_position(vmath.Vector3(-3.0, 10.0, 0.0))
        drawutil.draw_screen_text(vmath.Vector3(0, 100, 0), str(vmath.Vector3(spot_direction.x, spot_direction.y, -spot_direction.z)))
        self.spot_light2.set_direction(vmath.Vector3(spot_direction.x, spot_direction.y, -spot_direction.z))
        #self.spot_light2.set_direction(vmath.Vector3(0.0, -0.900168, -0.435542))
        drawutil.draw_sphere(spot_position, 0.1)
        drawutil.draw_line(spot_position, spot_position + spot_direction)

        transform = vmath.Transform()
        transform.translation = vmath.Vector3(0.0, 3.0, 2.0)
        transform.rotation = vmath.Quaternion.from_euler_angles(vmath.Vector3(0.0, game_time * 0.3, game_time * 0.5))
        drawutil.draw_perspective(transform, 1.0, 1.6, 3.0, 10.0)
        # drawutil.draw_orthographic(transform, -5.0, 5.0, -3.0, 3.0, 3.0, 10.0, vcolor.YELLOW)

        orientation = vmath.Quaternion.from_euler_angles(vmath.Vector3(0.0, game_time * 0.2, 0.0))
        ortho_transform, ortho_param = mathutil.perspective_to_bounding_orthographic(transform, 1.0, 1.6, 3.0, 10.0, orientation)
        drawutil.draw_orthographic(ortho_transform, *ortho_param, color=vcolor.YELLOW)

        orientation = vmath.Quaternion.from_euler_angles(vmath.Vector3(0.0, 0.0, 0.0))
        ortho_transform, ortho_param = mathutil.perspective_to_bounding_orthographic(transform, 1.0, 1.6, 3.0, 10.0, orientation)
        drawutil.draw_orthographic(ortho_transform, *ortho_param, color=vcolor.GREEN)

    def shutdown(self):
        self.console_server.shutdown()
