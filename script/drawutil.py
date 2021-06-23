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
