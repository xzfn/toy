import math

import toy
import vmath
from vmathlib import vcolor, vutil


def draw_line(position0, position1, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    toy.app.timed_geometry_builder.add_line(position0, position1, color, duration)

def draw_raw_line(position0, color0, position1, color1, duration=0.0):
    toy.app.timed_geometry_builder.add_raw_line(position0, color0, position1, color1, duration)

def draw_lines(lines, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    _add_line = toy.app.timed_geometry_builder.add_line
    for p0, p1 in lines:
        _add_line(p0, p1, color, duration)

def draw_lines_transform(transform, lines, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    _add_line = toy.app.timed_geometry_builder.add_line
    _transform_point = transform.transform_point
    for p0, p1 in lines:
        _add_line(_transform_point(p0), _transform_point(p1), color, duration)

def draw_color_lines(lines, duration=0.0):
    _add_line = toy.app.timed_geometry_builder.add_line
    for p0, p1, color in lines:
        _add_line(p0, p1, color, duration)

def draw_transform(transform, scale=1.0, duration=0.0):
    axis_x, axis_y, axis_z = transform.rotation.to_matrix3()
    origin = transform.translation
    scale_vec = transform.scale * scale
    _add_line = toy.app.timed_geometry_builder.add_line
    _add_line(origin, origin + axis_x * scale_vec.x, vmath.Vector3(1.0, 0.0, 0.0), duration)
    _add_line(origin, origin + axis_y * scale_vec.y, vmath.Vector3(0.0, 1.0, 0.0), duration)
    _add_line(origin, origin + axis_z * scale_vec.z, vmath.Vector3(0.0, 0.0, 1.0), duration)

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

def connect_point_pairs(points0, points1):
    lines = []
    for point0, point1 in zip(points0, points1):
        lines.append((point0, point1))
    return lines

def draw_circle(transform, radius=1.0, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    points = generate_unit_circle_points(16)
    transform = transform.copy()
    transform.scale *= radius
    points = [transform.transform_point(point) for point in points]
    lines = connect_points_loop(points)
    for line in lines:
        draw_line(line[0], line[1], color, duration)

def draw_split_line(position0, position1, color_end, color_split, t, duration=0.0):
    t_tail = vutil.clamp(t - 0.1, 0.0, 1.0)
    t_head = vutil.clamp(t + 0.1, 0.0, 1.0)
    split_position = vutil.lerp(position0, position1, t)
    tail_position = vutil.lerp(position0, position1, t_tail)
    head_position = vutil.lerp(position0, position1, t_head)
    draw_raw_line(position0, color_end, tail_position, color_end, duration)
    draw_raw_line(tail_position, color_end, split_position, color_split, duration)
    draw_raw_line(split_position, color_split, head_position, color_end, duration)
    draw_raw_line(head_position, color_end, position1, color_end, duration)

def draw_gradient_line(position0, position1, color_stops, duration=0.0):
    _draw_raw_line = toy.app.timed_geometry_builder.add_raw_line
    current_t, current_color = color_stops[0]
    current_position = vutil.lerp(position0, position1, current_t)
    for istop in range(1, len(color_stops)):
        t, color = color_stops[istop]
        position = vutil.lerp(position0, position1, t)
        _draw_raw_line(current_position, current_color, position, color, duration)
        current_t = t
        current_color = color
        current_position = position

def draw_travel_line(position0, position1, color_end, color_split, t, duration=0.0):
    if t < 0.1:
        color_seam = vutil.lerp(color_end, color_split, 1.0 - t / 0.1)
        color_stops = (
            (0.0, color_seam),
            (t, color_split),
            (t + 0.1, color_end),
            (t + 0.9, color_end),
            (1.0, color_seam)
        )
    elif t > 0.9:
        color_seam = vutil.lerp(color_end, color_split, (t - 0.9) / 0.1)
        color_stops = (
            (0.0, color_seam),
            (t - 0.9, color_end),
            (t - 0.1, color_end),
            (t, color_split),
            (1.0, color_seam)
        )
    else:
        color_stops = (
            (0.0, color_end),
            (t - 0.1, color_end),
            (t, color_split),
            (t + 0.1, color_end),
            (1.0, color_end)
        )
    draw_gradient_line(position0, position1, color_stops, duration)

def draw_perspective(transform, fov, aspect, z_near, z_far, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    t = math.tan(fov / 2.0)
    yn = z_near * t
    xn = yn * aspect
    neg_z_near = -z_near
    nears = [
        vmath.Vector3(-xn, -yn, neg_z_near),
        vmath.Vector3(xn, -yn, neg_z_near),
        vmath.Vector3(xn, yn, neg_z_near),
        vmath.Vector3(-xn, yn, neg_z_near),
    ]
    yf = z_far * t
    xf = yf * aspect
    neg_z_far = -z_far
    fars = [
        vmath.Vector3(-xf, -yf, neg_z_far),
        vmath.Vector3(xf, -yf, neg_z_far),
        vmath.Vector3(xf, yf, neg_z_far),
        vmath.Vector3(-xf, yf, neg_z_far),
    ]
    draw_transform(transform, 1.0, duration)
    draw_lines_transform(transform, connect_points_loop(nears), color, duration)
    draw_lines_transform(transform, connect_points_loop(fars), color, duration)
    draw_lines_transform(transform, connect_point_pairs(nears, fars), color, duration)

def draw_orthographic(transform, left, right, bottom, top, z_near, z_far, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    neg_z_near = -z_near
    nears = [
        vmath.Vector3(left, bottom, neg_z_near),
        vmath.Vector3(right, bottom, neg_z_near),
        vmath.Vector3(right, top, neg_z_near),
        vmath.Vector3(left, top, neg_z_near),
    ]
    neg_z_far = -z_far
    fars = [
        vmath.Vector3(left, bottom, neg_z_far),
        vmath.Vector3(right, bottom, neg_z_far),
        vmath.Vector3(right, top, neg_z_far),
        vmath.Vector3(left, top, neg_z_far),
    ]
    draw_transform(transform, 1.0, duration)
    draw_lines_transform(transform, connect_points_loop(nears), color, duration)
    draw_lines_transform(transform, connect_points_loop(fars), color, duration)
    draw_lines_transform(transform, connect_point_pairs(nears, fars), color, duration)

def draw_direction(origin, direction, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    aux_color = vcolor.complementary(color)
    draw_line(origin, origin + direction, color, duration)
    cone_origin = origin + vmath.Vector3(0.0, direction.y, 0.0)
    draw_line(origin, cone_origin, aux_color, duration)
    transform = vmath.Transform()
    transform.translation = cone_origin
    radius = vmath.Vector3(direction.x, 0.0, direction.z).length()
    draw_circle(transform, radius, aux_color, duration)

def draw_ray(origin, direction, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    draw_line(origin, origin + direction, color, duration)

def draw_ray_xy(origin, direction, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    end_position = origin + direction
    draw_line(origin, end_position, color, duration)
    normal = direction.cross(vmath.Vector3(0.0, 0.0, 1.0)).normalize()
    bottom_mid = vutil.lerp(end_position, origin, 0.2)
    side_length = (bottom_mid - end_position).length() * 0.5
    side_vec = normal * side_length
    draw_line(end_position, bottom_mid + side_vec, color, duration)
    draw_line(end_position, bottom_mid - side_vec, color, duration)

def draw_cone(transform, radius, height, color=vmath.Vector3(1.0, 0.0, 0.0), duration=0.0):
    draw_line(transform.translation, transform.transform_point(vmath.Vector3(0.0, height, 0.0)), color, duration)
    draw_circle(transform, radius, color, duration)
