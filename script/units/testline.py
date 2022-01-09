
import math
import re
from typing import ChainMap

import vmath
from vmathlib import vcolor, vutil

import drawutil
from unit_manager import Unit


def calc_dist(a, b, d, theta):
    c = math.sqrt(a*a - b*b)
    q = c - d
    p = c + d
    cos = math.cos(theta)
    # m
    mk = (2 * p * cos + 2 * q * cos) / (4 * a)
    mb = (- p * p + q * q) / (4 * a) + a
    # quadratic equation
    A = 1 - mk * mk
    B = - 2 * mk * mb + 2 * q * cos
    C = q * q - mb * mb
    # solve
    under_sqrt = B * B - 4 * A * C
    item = math.sqrt(under_sqrt)
    x = (-B + item) / (2 * A)
    return x


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

        self.test_ellipse(delta_time)

    def test_ellipse(self, delta_time):
        a = 5
        b = 3
        f = math.sqrt(a * a - b * b)
        theta = self.world.game_time
        x = a * math.cos(theta)
        y = b * math.sin(theta)
        drawutil.draw_cube(vmath.Vector3(x, 0.0, y))
        delta = 0
        #dist = math.sqrt((x - delta) ** 2 + y ** 2)
        dist = calc_dist(a, b, delta, theta)
        # drawutil.draw_sphere(vmath.Vector3(dist, 0.0, 0.0))
        drawutil.draw_text(vmath.Vector3(), str(dist))

        x2 = dist * math.cos(theta)
        y2 = dist * math.sin(theta)
        center = vmath.Vector3(-delta, 0.0, 0.0)
        point = center + vmath.Vector3(x2, 0.0, y2)
        circle_point = center + vmath.Vector3(math.cos(theta), 0.0, math.sin(theta))
        drawutil.draw_sphere(center, radius=0.1)
        drawutil.draw_sphere(point)
        drawutil.draw_sphere(circle_point, radius=0.2)
        drawutil.draw_line(center, point)
