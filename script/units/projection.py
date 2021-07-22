
import math

import vmath
from vmathlib import vcolor, vutil
import vmathlib

import drawutil
from unit_manager import Unit


class Projection(Unit):
    
    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)
        
    def tick(self, delta_time):
        game_time = self.world.game_time

        fov = 1.0
        aspect = 1.0
        z_near = 2.0
        z_far = 10.0
        projection = vmath.Matrix4.from_perspective(fov, aspect, z_near, z_far)

        t = math.tan(fov / 2.0)
        yn = z_near * t
        xn = yn * aspect
        nears = [
            vmath.Vector3(-xn, -yn, -z_near),
            vmath.Vector3(xn, -yn, -z_near),
            vmath.Vector3(xn, yn, -z_near),
            vmath.Vector3(-xn, yn, -z_near),
        ]
        yf = z_far * t
        xf = yf * aspect
        fars = [
            vmath.Vector3(-xf, -yf, -z_far),
            vmath.Vector3(xf, -yf, -z_far),
            vmath.Vector3(xf, yf, -z_far),
            vmath.Vector3(-xf, yf, -z_far),
        ]
        world_lines = []
        world_lines.extend(drawutil.connect_points_loop(nears))
        world_lines.extend(drawutil.connect_points_loop(fars))
        world_lines.extend(drawutil.connect_point_pairs(nears, fars))

        world_color_lines = []
        tripoints = [
            vmath.Vector3(1.0, 3.0, -4.0),
            vmath.Vector3(-3.0, 2.0, -8.0),
            vmath.Vector3(0.0, -1.0, -3.0)
        ]
        curcolor = vcolor.GREEN
        world_color_lines.extend([
            (tripoints[0], tripoints[1], curcolor),
            (tripoints[1], tripoints[2], curcolor),
            (tripoints[2], tripoints[0], curcolor),
        ])
        tripoints = [
            vmath.Vector3(5.0, 1.0, -10.0),
            vmath.Vector3(-1.0, 2.0, -4.0),
            vmath.Vector3(0.0, 0.0, -2.0)
        ]
        curcolor = vcolor.BLUE
        world_color_lines.extend([
            (tripoints[0], tripoints[1], curcolor),
            (tripoints[1], tripoints[2], curcolor),
            (tripoints[2], tripoints[0], curcolor),
        ])
        tripoints = [
            vmath.Vector3(4.0, -2.0, -8.0),
            vmath.Vector3(-2.0, -6.0, -9.0),
            vmath.Vector3(-1.0, -2.0, -7.0)
        ]
        curcolor = vcolor.CYAN
        world_color_lines.extend([
            (tripoints[0], tripoints[1], curcolor),
            (tripoints[1], tripoints[2], curcolor),
            (tripoints[2], tripoints[0], curcolor),
        ])

        drawutil.draw_lines(world_lines, vcolor.RED)
        drawutil.draw_color_lines(world_color_lines)

        identity = vmath.Matrix4()
        identity.m22 = -1.0

        alpha = vutil.fract(game_time * 0.05)
        t = vutil.half_pause_ping_pong(alpha)
        custom = vutil.lerp_matrix(projection, identity, t)

        _project_point = custom.project_point
        def _process_point(p):
            p_proj = _project_point(p)
            return vmath.Vector3(p_proj.x, p_proj.y, -p_proj.z)

        projected_lines = [(_process_point(p0), _process_point(p1)) for p0, p1 in world_lines]
        drawutil.draw_lines(projected_lines, vcolor.MAGENTA)
        projected_color_lines = [(_process_point(p0), _process_point(p1), c) for p0, p1, c in world_color_lines]
        drawutil.draw_color_lines(projected_color_lines)

    def render(self):
        pass
