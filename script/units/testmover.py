
import math
import toy

import vmath
from vmathlib import vcolor, vutil

import drawutil
from unit_manager import Unit


class Testmover(Unit):
    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)

    def tick(self, delta_time):
        game_time = self.world.game_time
        mover = BaseMover()
        mover.start_pos = vmath.Vector3(0, 0, 0)
        mover.end_pos = vmath.Vector3(5, 5, 0)
        segment_count = 20
        polyline = []
        for i in range(segment_count + 1):
            alpha = i / segment_count
            mover.update(alpha)
            polyline.append(mover.current_pos)
            drawutil.draw_cube(mover.current_pos, 0.1, color=vcolor.GREEN)
        drawutil.draw_polyline(polyline)

        t = vutil.fract(game_time)
        mover.update(t)
        position = mover.current_pos
        drawutil.draw_cube(position, 0.2, color=vcolor.BLUE)


class BaseMover(object):
    def __init__(self):
        self.start_pos = vmath.Vector3()
        self.end_pos = vmath.Vector3()
        self.current_pos = self.start_pos

    def update(self, alpha):
        alpha = vutil.alpha_ease_in_out(alpha, 3)
        alpha = vutil.alpha_ease_in_out(alpha, 1.0 / 3.0)
        alpha = vutil.alpha_ease_out(alpha, 4)
        alpha_y = vutil.alpha_ease_in(alpha, 2)
        current_pos_xz = vutil.lerp(self.start_pos, self.end_pos, alpha)
        current_pos_y = vutil.lerp(self.start_pos.y, self.end_pos.y, alpha_y)
        self.current_pos = vmath.Vector3(current_pos_xz.x, current_pos_y, current_pos_xz.z)
