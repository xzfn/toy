
import math
from bisect import bisect_left
from typing import Match

import vmath
from vmathlib import vcolor, vutil

import toy
import keycodes

import drawutil

from unit_manager import Unit


class StoneSkipping(object):
    def __init__(self, start_pos, end_pos, duration, segment_count=1):
        self.start_pos = start_pos
        self.end_pos = end_pos
        self.duration = duration
        self.segment_count = segment_count

        self._time = 0.0
        self._complete = False

        self._last_segment = 1

        self._flat_delta_position = end_pos - start_pos
        self._flat_delta_position.y = 0.0

        self.height_base_above = 1
        height_ratio = 1.8
        endpoint_ratio = 0.8

        if self.segment_count > 1:
            # 抛物线的切割比例 parabola_ratio: [0.8, 1.0, 1.0, 0.8]
            self.endpoint_ratio = endpoint_ratio
            parabola_ratios = [endpoint_ratio]
            parabola_ratios.extend([1.0] * (segment_count - 2))
            parabola_ratios.append(endpoint_ratio)
            # 高度比例 height_ratios: [a^3, a^2, a, 1]
            height_ratios = [height_ratio**i for i in range(segment_count - 1, -1, -1)]
            self.frame_height_base = self.end_pos.y + self.height_base_above
            # 时间比例 time_ratios
            temp_time_ratios = [math.sqrt(height_ratio) for height_ratio in height_ratios]
            time_ratios = [ratio_a * ratio_b for ratio_a, ratio_b in zip(parabola_ratios, temp_time_ratios)]
            norm = sum(time_ratios)
            time_ratios = [ratio / norm for ratio in time_ratios]
            # 分割 split_frames: [0.0, 0.4, 0.7, 0.9] (1.0)
            split_frames = []
            total_ratio = 0.0
            for time_ratio in time_ratios:
                split_frames.append(total_ratio)
                total_ratio += time_ratio
            # 保存
            self._split_frames = split_frames
            self._height_ratios = height_ratios
            print(height_ratios)

    def set_end_pos(self, end_pos):
        self.end_pos = end_pos

    def is_complete(self):
        return self._complete

    def update(self, delta_time):
        self._time += delta_time
        alpha = self._time / self.duration
        if alpha >= 1.0:
            self._complete = True
            alpha = 1.0
        if alpha == 1.0:
            return self.end_pos

        if self.segment_count == 1:
            base_pos = vutil.lerp(self.start_pos, self.end_pos, alpha)
            delta_height_top = self.height_base_above
            delta_height = (-alpha * alpha + alpha) * 4.0 * delta_height_top
            res_pos = base_pos + vmath.Vector3(0.0, delta_height, 0.0)
            return res_pos

        split_frames = self._split_frames
        current_segment = bisect_left(split_frames, alpha)
        if self._last_segment != current_segment:
            # NOTE skip point notify
            print('skip point', alpha, current_segment)
            self._last_segment = current_segment
        if current_segment == 0:
            return self.start_pos

        left_segment = current_segment - 1
        left_frame = split_frames[left_segment]
        if current_segment == self.segment_count:
            right_frame = 1.0
        else:
            right_frame = split_frames[current_segment]

        frame_alpha = (alpha - left_frame) / (right_frame - left_frame)

        frame_start_position = self._calc_skip_position(left_frame)
        frame_end_position = self._calc_skip_position(right_frame)
        # print('ss', frame_start_position, frame_end_position)

        frame_height = self._height_ratios[left_segment] * self.frame_height_base
        endpoint_delta_height_top = frame_height - self.frame_height_base * 0.5

        if left_frame == 0:
            # start
            delta_height_top = endpoint_delta_height_top
        elif right_frame == 1.0:
            # end
            delta_height_top = endpoint_delta_height_top
        else:
            # middle
            delta_height_top = frame_height

        base_pos = vutil.lerp(frame_start_position, frame_end_position, frame_alpha)
        delta_height = (-frame_alpha * frame_alpha + frame_alpha) * 4.0 * delta_height_top
        res_pos = base_pos + vmath.Vector3(0.0, delta_height, 0.0)
        return res_pos

    def _calc_skip_position(self, alpha):
        if alpha == 0.0:
            return self.start_pos
        elif alpha == 1.0:
            return self.end_pos
        else:
            position = vutil.lerp(self.start_pos, self.end_pos, alpha)
            position.y = 0.0  # NOTE sea level correction
            return position


class TestStoneSkipping(Unit):
    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)
        self._movements = []

    def tick(self, delta_time):
        gametime = self.world.game_time
        alpha = vutil.ping_pong(vutil.fract(gametime))
        end_pos = vmath.Vector3(0.0, 3.0, 30.0)
        alpha = 0.0
        for movement in self._movements.copy():
            movement.set_end_pos(end_pos + vmath.Vector3(0.0, 0.0, alpha * 5.0))
            new_pos = movement.update(delta_time)
            drawutil.draw_cube(new_pos, 0.2)
            drawutil.draw_sphere(movement.start_pos)
            drawutil.draw_sphere(movement.end_pos)
            pos = new_pos.copy()
            pos.y = 0.0
            drawutil.draw_sphere(pos, 0.2)

            if movement.is_complete():
                self._movements.remove(movement)

        input_manager = toy.app.input_manager
        if input_manager.get_key_down(keycodes.VK_F):
            movement = StoneSkipping(
                vmath.Vector3(0.0, 3.0, 0.0),
                end_pos,
                3.0,
                3
            )
            self._movements.append(movement)
