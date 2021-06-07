
import time
import math
import sys

import numpy as np

import toy
import vmath

import remoteconsole.server

#import toyqt
from world import World
import drawutil

from units.player import Player


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
        toy.bind_on_key_down(self.on_key_down)
        #toyqt.startup()
        #toyqt.qtwindow.on_shoot = self.on_shoot
        self.world = World()
        self.world.unit_manager.create_unit(Player)

    def on_shoot(self):
        drawutil.draw_line(vmath.Vector3(0.0, 0.0, 0.0), vmath.Vector3(10.0, 10.0, 10.0), vmath.Vector3(1.0, 1.0, 0.0), 5.0)

    def on_key_down(self, key):
        print('py on_key_down', key)
        if key == ord('R'):
            import retroreload
            retroreload.retroreload(sys.modules[__name__])


    def update(self):
        self.console_server.service()
        time.sleep(0.001)
        #toyqt.update()

        mouse_dx, mouse_dy = toy.get_input_manager().get_mouse_move()
        mouse_wheel = toy.get_input_manager().get_mouse_wheel()

        delta_time = toy.get_delta_time()
        self.world.tick(delta_time)

        drawutil.draw_ground_grid()
        transform = vmath.Transform()
        transform.translation = vmath.Vector3(0.0, 0.0, -10.0)
        drawutil.draw_transform(transform)

        mouse_x, mouse_y = toy.get_input_manager().get_mouse_position()
        text = '{:4} {:4}'.format(mouse_x, mouse_y)
        drawutil.draw_screen_text(vmath.Vector3(mouse_x, mouse_y, 0.0), text)

        self.world.render()

    def shutdown(self):
        self.console_server.shutdown()
