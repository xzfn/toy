
import math
import re

import vmath
from vmathlib import vcolor, vutil

import drawutil
from unit_manager import Unit


def str_to_float_list(s):
    return [float(f) for f in s.split(',')]


def polyline_to_origin(polyline):
    first = polyline[0]
    new_polyline = []
    for point in polyline:
        new_polyline.append(point - first)
    return new_polyline


class Testline(Unit):
    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)
        self.polyline = None

    def load_polyline(self):
        polyline = []
        with open('../../log.txt', 'r') as f:
            for line in f:
                if line.startswith('ddd'):
                    s = re.match('ddd <(.*?)>', line).group(1)
                    v = vmath.Vector3(*str_to_float_list(s))
                    polyline.append(v)
        polyline = polyline_to_origin(polyline)
        self.polyline = polyline

    def tick(self, delta_time):
        if self.polyline:
            drawutil.draw_polyline(self.polyline)
            for point in self.polyline:
                drawutil.draw_cube(point, length=0.2, color=vcolor.GREEN)
            drawutil.draw_cube(self.polyline[0], color=vcolor.CYAN)
