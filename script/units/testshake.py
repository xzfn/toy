import math

import toy
import keycodes

import vmath
from vmathlib import vcolor, vutil

import drawutil

from unit_manager import Unit


def mode_calc_sin(current_time, frequency):
    return math.sin(2.0 * math.pi * frequency * current_time)


def compose_scalar(magnitude, direction):
    return magnitude * direction


class SimpleShaker:
    def __init__(self):
        self.amplitude = 0.0
        self.direction = vmath.Vector3(0.0, 0.0, 1.0)
        self.frequency = 1.0
        self.loop_count = 0
        self.decay_coeff = 0.0

        self._current_time = 0.0
        self._duration = 0.0

    def update(self, delta_time):
        if self._current_time >= self._duration:
            return
        current_time = self._current_time
        self._current_time += delta_time
        base = mode_calc_sin(current_time, self.frequency)
        decay = 2 ** (-current_time / self._duration * self.decay_coeff)
        magnitude = base * self.amplitude * decay
        res_translation = compose_scalar(magnitude, self.direction)
        transform = vmath.Transform()
        transform.translation = res_translation
        return transform

    def start_shake(self):
        if self.frequency <= 0.0:
            return
        self._current_time = 0.0
        self._duration = self.loop_count / self.frequency

    def stop_shake(self):
        self._duration = 0.0


class ShakeAxis:
    def __init__(self):
        self.amplitude = 0.0
        self.frequency = 1.0


class ShakeData:
    def __init__(self):
        self.axis_x = None
        self.axis_y = None
        self.axis_z = None
        self.axis_pitch = None
        self.axis_yaw = None
        self.axis_roll = None

        self.duration = 0.0
        self.blend_in = 0.0
        self.blend_out = 0.0


class Shaker:
    def __init__(self):
        self.shake_data = None

    def update(self, delta_time):
        pass


class ShakeManager:
    def __init__(self):
        self._shakers = []

    def update(self, delta_time):
        for shaker in self._shakers:
            shaker.update(delta_time)

    def play_shake(self, shake_data, scale, shake_space, custom_direction):
        shaker = Shaker()
        shaker.shake_data = shake_data
        self._shakers.append(shaker)


class Testshake(Unit):
    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)
        self.simple_shaker = SimpleShaker()
        self.shake_manager = ShakeManager()

    def tick(self, delta_time):
        self.tick_simple_shaker(delta_time)
        self.tick_shaker(delta_time)

    def tick_simple_shaker(self, delta_time):
        input_manager = toy.app.input_manager
        if input_manager.get_key_down(keycodes.VK_F):
            print('shake')
            shaker = self.simple_shaker
            shaker.amplitude = 1.0
            shaker.direction = vmath.Vector3(0.0, 0.5, 1.0).normalize()
            shaker.frequency = 5.0
            shaker.loop_count = 4
            shaker.decay_coeff = 5.0
            shaker.start_shake()

        delta_transform = self.simple_shaker.update(delta_time)
        if not delta_transform:
            delta_transform = vmath.Transform()

        camera_transform = vmath.Transform()
        camera_transform.translation = vmath.Vector3(2.0, 2.0, 2.0)

        new_transform = delta_transform * camera_transform
        drawutil.draw_transform(camera_transform)
        drawutil.draw_transform(new_transform)

    def tick_shaker(self, delta_time):
        shake_manager = self.shake_manager
        delta_transform = shake_manager.update(delta_time)

