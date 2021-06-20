
import os
import time
import math
import sys

import numpy as np

import toy
import vmath
from vmathlib import vcolors, vutil

import remoteconsole.server
import retroreload

#import toyqt
from world import World
import drawutil

from units.player import Player
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
        self.world.unit_manager.create_unit(Player)

        self.texture = toy.Texture.create('resource/cube.png')
        self.pipeline = toy.BasicPipeline.create('shader/basic.vert.spv', 'shader/basic.frag.spv')
        self.material = toy.Material.create(self.pipeline, self.texture)
        mesh_data = build_quad_mesh_data()
        self.mesh = toy.Mesh.create(mesh_data)

        builder = toy.GeometryBuilder()
        builder.add_cube(vmath.Vector3(), 3.0, vcolors.cyan)
        builder.add_cube(vmath.Vector3(1.0, 0.0, 0.0), 0.5, vcolors.magenta)
        geometry_mesh_data = builder.build_data()
        self.geometry_mesh = toy.GeometryMesh.create(geometry_mesh_data)

    def on_shoot(self):
        drawutil.draw_line(vmath.Vector3(0.0, 0.0, 0.0), vmath.Vector3(10.0, 10.0, 10.0), vmath.Vector3(1.0, 1.0, 0.0), 5.0)

    def on_key_down(self, key):
        print('py on_key_down', key)
        if key == keycodes.VK_F5:
            main_script_folder = os.path.dirname(sys.modules[__name__].__file__)
            retroreload.retroreload_script_folders([main_script_folder])

    def update(self):
        self.console_server.service()
        time.sleep(0.001)
        #toyqt.update()

        mouse_dx, mouse_dy = toy.app.input_manager.get_mouse_move()
        mouse_wheel = toy.app.input_manager.get_mouse_wheel()

        delta_time = toy.app.delta_time
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
        text = '{:4} {:4}'.format(mouse_x, mouse_y)
        drawutil.draw_screen_text(vmath.Vector3(mouse_x, mouse_y, 0.0), text)
        drawutil.draw_text(vmath.Vector3(5, 5, 5), '(*{}*)'.format(text))
        self.world.render()

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

        transform.translation = vmath.Vector3(0.0, 3.0, 0.0)
        transform.rotation = vmath.Quaternion.from_angle_axis(self.world.game_time, vmath.Vector3(0.0, 0.0, 1.0))
        toy.app.render_manager.add_geometry_mesh(self.geometry_mesh, transform.to_matrix4(), toy.app.material_lines)
        transform.translation = vmath.Vector3(0.0, 10.0, 0.0)
        transform.scale = vmath.Vector3(1.0, 1.0, 1.0) * math.modf(self.world.game_time)[0] + vmath.Vector3(1, 1, 1)
        toy.app.render_manager.add_geometry_mesh(self.geometry_mesh, transform.to_matrix4(), toy.app.material_lines)

    def shutdown(self):
        self.console_server.shutdown()
