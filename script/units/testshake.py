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


def advance_cursor(current, frequency, delta_time):
    current += frequency * delta_time
    return current


class ShakeAxis:
    def __init__(self, amplitude, frequency):
        self.amplitude = amplitude
        self.frequency = frequency


class ShakeData:
    def __init__(self):
        self.instance_type = 0
        self.single_instance = False

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
        self.scale = 1.0
        self.shake_space = 0
        self.custom_direction = None

        self.current_location_cursor = vmath.Vector3()  # freq * time
        self.current_rotation_cursor = vmath.Vector3()  # freq * time
        self.remaining_time = 0.0
        self.current_blending_in_time = 0.0
        self.is_blending_in = False

    def update(self, delta_time):
        if self.remaining_time <= 0.0:
            # 结束了
            return
        self.remaining_time -= delta_time
        if self.remaining_time < 0.0:
            self.remaining_time = 0.0

        shake_data = self.shake_data
        blend_in_coeff = 1.0
        if self.is_blending_in:
            # 正在blend_in
            self.current_blending_in_time += delta_time
            if self.current_blending_in_time < shake_data.blend_in:
                blend_in_coeff = self.current_blending_in_time / shake_data.blend_in
            else:
                self.is_blending_in = False

        blend_out_coeff = 1.0
        if self.remaining_time < shake_data.blend_out:
            # 剩余时间不多了，需要blend_out
            blend_out_coeff = self.remaining_time / shake_data.blend_out

        blend_coeff = min(blend_in_coeff, blend_out_coeff)

        res_x = 0.0
        if shake_data.axis_x and shake_data.axis_x.amplitude > 0.0:
            self.current_location_cursor.x += shake_data.axis_x.frequency * delta_time
            res_x = math.sin(2.0 * math.pi * self.current_location_cursor.x) * shake_data.axis_x.amplitude

        res_y = 0.0
        if shake_data.axis_y and shake_data.axis_y.amplitude > 0.0:
            self.current_location_cursor.y += shake_data.axis_y.frequency * delta_time
            res_y = math.sin(2.0 * math.pi * self.current_location_cursor.y) * shake_data.axis_y.amplitude

        res_z = 0.0
        if shake_data.axis_z and shake_data.axis_z.amplitude > 0.0:
            self.current_location_cursor.z += shake_data.axis_z.frequency * delta_time
            res_z = math.sin(2.0 * math.pi * self.current_location_cursor.z) * shake_data.axis_z.amplitude

        res_pitch = 0.0
        if shake_data.axis_pitch and shake_data.axis_pitch.amplitude > 0.0:
            self.current_rotation_cursor.x += shake_data.axis_pitch.frequency * delta_time
            res_pitch = math.sin(2.0 * math.pi * self.current_rotation_cursor.x) * shake_data.axis_pitch.amplitude

        res_yaw = 0.0
        if shake_data.axis_yaw and shake_data.axis_yaw.amplitude > 0.0:
            self.current_rotation_cursor.y += shake_data.axis_yaw.frequency * delta_time
            res_yaw = math.sin(2.0 * math.pi * self.current_rotation_cursor.y) * shake_data.axis_yaw.amplitude

        res_roll = 0.0
        if shake_data.axis_roll and shake_data.axis_roll.amplitude > 0.0:
            self.current_rotation_cursor.z += shake_data.axis_roll.frequency * delta_time
            res_roll = math.sin(2.0 * math.pi * self.current_rotation_cursor.z) * shake_data.axis_roll.amplitude

        delta_transform = vmath.Transform()
        delta_transform.translation = vmath.Vector3(res_x, res_y, res_z) * blend_coeff
        delta_transform.rotation = vmath.Quaternion.from_euler_angles(vmath.Vector3(res_pitch, res_yaw, res_roll) * blend_coeff)
        return delta_transform

    def start_shake(self, shake_data, scale, shake_space, custom_direction):
        self.shake_data = shake_data
        if self.remaining_time > 0.0:
            # 正在震，刷新参数，继续震
            # 如果正在blend in或者平稳，不用处理
            # 如果正在blend out，需要从当前位置重新blend_in
            if self.remaining_time < shake_data.blend_out:
                self.is_blending_in = True
                self.current_blending_in_time = shake_data.blend_in * (self.remaining_time / shake_data.blend_out)
        else:
            # 从头开始震
            self.is_blending_in = True
            self.current_blending_in_time = 0.0

        # 剩余时间直接用新的
        self.remaining_time = shake_data.duration
        self.scale = scale
        self.shake_space = shake_space
        self.custom_direction = custom_direction


class ShakeManager:
    def __init__(self):
        self.shakers = []

    def update(self, delta_time):
        combined_delta_transform = vmath.Transform()
        finished = []
        for shaker in self.shakers:
            delta_transform = shaker.update(delta_time)
            if delta_transform:
                combined_delta_transform = delta_transform * combined_delta_transform
            else:
                finished.append(shaker)

        for shaker in finished:
            self.shakers.remove(shaker)

        return combined_delta_transform

    def play_shake(self, shake_data, scale, shake_space, custom_direction):
        shaker = None
        if shake_data.single_instance:
            # 寻找可能已经存在的同类型shaker
            for skr in self.shakers:
                if skr.shake_data.instance_type == shake_data.instance_type:
                    shaker = skr
                    print('found same type')
                    break
        if not shaker:
            shaker = Shaker()
            self.shakers.append(shaker)

        shaker.start_shake(shake_data, scale, shake_space, custom_direction)


class Testshake(Unit):
    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)
        self.simple_shaker = SimpleShaker()
        self.shake_manager = ShakeManager()

    def tick(self, delta_time):
        self.tick_simple_shaker(delta_time)
        self.tick_shake_manager(delta_time)

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

        new_transform = camera_transform * delta_transform
        drawutil.draw_transform(camera_transform)
        drawutil.draw_transform(new_transform)

    def tick_shake_manager(self, delta_time):
        shake_manager = self.shake_manager
        input_manager = toy.app.input_manager
        if input_manager.get_key_down(keycodes.VK_R):
            print('shake 2')
            shake_data = ShakeData()
            shake_data.axis_y = ShakeAxis(1.0, 1.0)
            shake_data.axis_roll = ShakeAxis(1.0, 2.0)
            shake_data.duration = 3.0
            shake_data.blend_in = 1.0
            shake_data.blend_out = 1.0
            shake_data.instance_type = 1
            shake_data.single_instance = True
            
            if input_manager.get_key(keycodes.VK_CONTROL):
                shake_data.single_instance = False

            shake_manager.play_shake(shake_data, 1.0, None, None)

        delta_transform = shake_manager.update(delta_time)

        camera_transform = vmath.Transform()
        camera_transform.translation = vmath.Vector3(2.0, 4.0, 2.0)

        new_transform = camera_transform * delta_transform
        drawutil.draw_transform(camera_transform)
        drawutil.draw_transform(new_transform)
