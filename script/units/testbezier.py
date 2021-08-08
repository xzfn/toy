
import math

import vmath
from vmathlib import vcolor, vutil

import drawutil
from unit_manager import Unit


def bezier(points, t):
    s = 1 - t
    coeffs = [s**3, 3*t*s**2, 3*t**2*s, t**3]
    res = type(points[0])()
    for i in range(4):
        res += coeffs[i] * points[i]
    return res


def _bezier_de_casteljau_helper(points, i, k, t):
    if k == 0:
        return points[i]
    pa = _bezier_de_casteljau_helper(points, i, k-1, t)
    pb = _bezier_de_casteljau_helper(points, i+1, k-1, t)
    return (1-t) * pa + t * pb


def bezier_de_casteljau(points, t):
    k = len(points) - 1
    return _bezier_de_casteljau_helper(points, 0, k, t)


def bezier_de_casteljau_non_recursive(points, t):
    points = points.copy()
    n = len(points)
    for step in range(n-1, 0, -1):
        for i in range(step):
            points[i] = points[i] * (1 - t) + points[i + 1] * t
    return points[0]


class Testbezier(Unit):
    def tick(self, delta_time):
        game_time = self.world.game_time
        control_points = [
            vmath.Vector3(0, 0, 0),
            vmath.Vector3(1, 1, 1),
            vmath.Vector3(2, 0, -2),
            vmath.Vector3(3, 0, 0)
        ]
        control_points[2].x = math.cos(game_time) * 2.0 + 3.0
        control_points[2].z = math.sin(game_time) * 2.0
        drawutil.draw_polyline(control_points, color=vcolor.GREEN)
        line_points = []
        line_points2 = []
        count = 20
        for i in range(count + 1):
            t = i / count
            line_points.append(bezier(control_points, t))
            line_points2.append(bezier_de_casteljau_non_recursive(control_points, t))

        drawutil.draw_polyline(line_points, color=vcolor.MAGENTA)
        drawutil.draw_polyline(line_points2, color=vcolor.CYAN)
