
import math

import vmath


def perspective_to_bounding_orthographic(transform, fov, aspect, z_near, z_far, orientation):
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

    reverse_transform = vmath.Transform()
    reverse_transform.rotation = orientation
    concat_transform = reverse_transform.inverse() * transform

    _transform_point = concat_transform.transform_point
    points = []
    points.extend([_transform_point(p) for p in nears])
    points.extend([_transform_point(p) for p in fars])

    xs = [p.x for p in points]
    ys = [p.y for p in points]
    zs = [p.z for p in points]

    x_min = min(xs)
    x_max = max(xs)
    y_min = min(ys)
    y_max = max(ys)
    z_min = min(zs)
    z_max = max(zs)

    ortho_x = (x_min + x_max) / 2.0
    ortho_y = (y_min + y_max) / 2.0
    ortho_z = z_max

    abs_left = (x_max - x_min) / 2.0
    abs_bottom = (y_max - y_min) / 2.0

    ortho_left = -abs_left
    ortho_right = abs_left
    ortho_bottom = -abs_bottom
    ortho_top = abs_bottom
    ortho_z_near = 0.0
    ortho_z_far = z_max - z_min

    ortho_transform = vmath.Transform()
    ortho_transform.translation = orientation.transform_point(vmath.Vector3(ortho_x, ortho_y, ortho_z))
    ortho_transform.rotation = orientation
    ortho_param = (ortho_left, ortho_right, ortho_bottom, ortho_top, ortho_z_near, ortho_z_far)

    return (ortho_transform, ortho_param)
