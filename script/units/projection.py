
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
        z_near = 1.0
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

        drawutil.draw_lines(world_lines, vcolor.RED)

        identity = vmath.Matrix4()

        t = vutil.ping_pong(vutil.fract(game_time * 0.2))
        #t = 0
        custom = vutil.lerp_matrix(projection, identity, t)

        _project_point = custom.project_point
        projected_lines = [(_project_point(p0), _project_point(p1)) for p0, p1 in world_lines]
        drawutil.draw_lines(projected_lines, vcolor.MAGENTA)


    def render(self):
        pass
