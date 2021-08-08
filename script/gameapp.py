
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
from units.projection import Projection
from units.cameraman import Cameraman
from units.lightman import Lightman
from units.shadowman import Shadowman
from units.testman import Testman
from units.testgltf import Testgltf
from units.testnormal import Testnormal
from units.testculling import Testculling
from units.testbezier import Testbezier

import keycodes


class App:
    def __init__(self):
        server_address = ('127.0.0.1', 9000)
        self.console_environment = {}
        self.console_server = remoteconsole.server.ConsoleServer(server_address)
        console_handler = remoteconsole.server.PythonCodeHandler(self.console_environment)
        self.console_server.set_request_handler(console_handler)
        self.world = None

    def startup(self):
        toy.app.bind_key_down(self.on_key_down)
        self.world = World()

        # common render resources
        self.texture = toy.Texture.create('resource/cube.png')
        self.pipeline = toy.BasicPipeline.create('shader/basic.vert.spv', 'shader/basic.frag.spv')
        self.material = toy.Material.create(self.pipeline, self.texture)

        self.red_texture = toy.Texture.create_color(vcolor.RED)
        self.green_texture = toy.Texture.create_color(vcolor.GREEN)
        self.blue_texture = toy.Texture.create_color(vcolor.BLUE)
        self.red_material = toy.Material.create(self.pipeline, self.red_texture)
        self.green_material = toy.Material.create(self.pipeline, self.green_texture)
        self.blue_material = toy.Material.create(self.pipeline, self.blue_texture)
        self.rgb_materials = [self.red_material, self.green_material, self.blue_material]

        # units
        # self.world.unit_manager.create_unit(Player)
        # self.world.unit_manager.create_unit(Enemy)
        # self.world.unit_manager.create_unit(Projection)
        # self.world.unit_manager.create_unit(Cameraman)
        # self.world.unit_manager.create_unit(Lightman)
        # self.world.unit_manager.create_unit(Shadowman)
        # self.world.unit_manager.create_unit(Testman)
        # self.world.unit_manager.create_unit(Testgltf)
        # self.world.unit_manager.create_unit(Testnormal)
        # self.world.unit_manager.create_unit(Testculling)
        self.world.unit_manager.create_unit(Testbezier)

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

        delta_time = toy.app.delta_time

        self.draw_frame_info()
        self.draw_helper()

        self.world.tick(delta_time)

        self.world.render()

    def draw_frame_info(self):
        delta_time = toy.app.delta_time
        drawutil.draw_screen_text(vmath.Vector3(0, 20, 0), 'dt {:6.4f}'.format(delta_time), color=vcolors.white)

    def draw_helper(self):
        drawutil.draw_ground_grid()
        length = 10.0
        drawutil.draw_text(vutil.VEC3_X * length, 'X', color=vcolor.RED)
        drawutil.draw_text(vutil.VEC3_Z * length, 'Z', color=vcolor.BLUE)
        drawutil.draw_text(-vutil.VEC3_X * length, '-X', color=vcolor.RED)
        drawutil.draw_text(-vutil.VEC3_Z * length, '-Z', color=vcolor.BLUE)
        draw_y = True
        if draw_y:
            drawutil.draw_line(vutil.VEC3_ZERO, vmath.Vector3(0.0, 10.0, 0.0), color=vcolor.GREEN)
            drawutil.draw_text(vutil.VEC3_Y * length, 'Y', color=vcolor.GREEN)

    def shutdown(self):
        self.console_server.shutdown()
