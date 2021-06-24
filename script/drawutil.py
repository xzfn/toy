import math

import toy
import vmath


def draw_line(position0, position1, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    toy.app.timed_geometry_builder.add_line(position0, position1, color, duration)

def draw_transform(transform, duration=0.0):
    axis_x, axis_y, axis_z = transform.rotation.to_matrix3()
    origin = transform.translation
    length = 1.0
    _add_line = toy.app.timed_geometry_builder.add_line
    _add_line(origin, origin + axis_x * length, vmath.Vector3(1.0, 0.0, 0.0), duration)
    _add_line(origin, origin + axis_y * length, vmath.Vector3(0.0, 1.0, 0.0), duration)
    _add_line(origin, origin + axis_z * length, vmath.Vector3(0.0, 0.0, 1.0), duration)

def draw_ground_grid(step=1.0, count=10, color=vmath.Vector3(0.5, 0.5, 0.5), duration=0.0):
    side = step * count
    _add_line = toy.app.timed_geometry_builder.add_line
    for i in range(-count, count + 1):
        current = step * i
        _add_line(vmath.Vector3(current, 0.0, -side), vmath.Vector3(current, 0.0, side), color, duration)
        _add_line(vmath.Vector3(-side, 0.0, current), vmath.Vector3(side, 0.0, current), color, duration)
    _add_line(vmath.Vector3(-side, 0.0, 0.0), vmath.Vector3(side, 0.0, 0.0), vmath.Vector3(1.0, 0.0, 0.0), duration)
    _add_line(vmath.Vector3(0.0, 0.0, -side), vmath.Vector3(0.0, 0.0, side), vmath.Vector3(0.0, 0.0, 1.0), duration)

def draw_text(position, text, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    toy.app.timed_text_builder.add_text(position, text, color, duration)

def draw_screen_text(position, text, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    toy.app.timed_text_builder.add_screen_text(position, text, color, duration)

def draw_sphere(position, radius=1.0, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    toy.app.timed_geometry_builder.add_sphere(position, radius, color, duration)

def draw_cube(position, length=1.0, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    toy.app.timed_geometry_builder.add_cube(position, length, color, duration)

def generate_unit_circle_points(segments):
    points = []
    delta_angle = math.pi * 2.0 / segments
    for i in range(segments):
        theta = delta_angle * i
        x = math.cos(theta)
        z = math.sin(theta)
        points.append(vmath.Vector3(x, 0.0, z))
    return points

def connect_points_loop(points):
    lines = []
    point_count = len(points)
    for i in range(point_count):
        lines.append((points[i], points[(i + 1) % point_count]))
    return lines

def draw_circle(transform, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    points = generate_unit_circle_points(16)
    points = [transform.transform_point(point) for point in points]
    lines = connect_points_loop(points)
    for line in lines:
        draw_line(line[0], line[1], color, duration)
