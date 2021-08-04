import math

import toy

import vmath
from vmathlib import vcolor, vutil, vcolors
import vmathlib

import drawutil
from unit_manager import Unit

import toyentry


class AABB:
    def __init__(self, center, extents):
        self.center = center
        self.extents = extents


class OBB:
    def __init__(self, center, axes, extents):
        self.center = center
        self.axes = axes
        self.extents = extents


def get_element(m, col, row):
    return getattr(m, 'm{}{}'.format(col, row))


def transform_aabb(transform, aabb):
    old_center = [aabb.center.x, aabb.center.y, aabb.center.z]
    old_extents = [aabb.extents.x, aabb.extents.y, aabb.extents.z]
    translation = [get_element(transform, 3, 0), get_element(transform, 3, 1), get_element(transform, 3, 2)]
    new_center = translation.copy()
    new_extents = [0.0, 0.0, 0.0]
    for col in range(3):
        for row in range(3):
            new_center[row] += get_element(transform, col, row) * old_center[col]
            new_extents[row] += abs(get_element(transform, col, row)) * old_extents[col]
    return AABB(vmath.Vector3(*new_center), vmath.Vector3(*new_extents))


def transform_aabb_from_book(transform, aabb):
    # Real-Time Collision Detection book 4.2, and Arvo1990
    # the matrix is read as m[row][col], so the swap
    old_center = [aabb.center.x, aabb.center.y, aabb.center.z]
    old_extents = [aabb.extents.x, aabb.extents.y, aabb.extents.z]
    new_center = [0.0, 0.0, 0.0]
    new_extents = [0.0, 0.0, 0.0]
    for i in range(3):
        new_center[i] = get_element(transform, 3, i)
        new_extents[i] = 0.0
        for j in range(3):
            new_center[i] += get_element(transform, j, i) * old_center[j]  # m[i][j] -> m[j][i]
            new_extents[i] += abs(get_element(transform, j, i)) * old_extents[j]
    return AABB(vmath.Vector3(*new_center), vmath.Vector3(*new_extents))


# from filament Box.cpp. Jim Arvo Graphics Gems (1990)
# see also Real-Time Collision Detection book 4.2
def transform_aabb_minmax(transform, aabb):
    mins_vec = aabb.center - aabb.extents
    maxs_vec = aabb.center + aabb.extents
    mins = [mins_vec.x, mins_vec.y, mins_vec.z]
    maxs = [maxs_vec.x, maxs_vec.y, maxs_vec.z]

    translation = [get_element(transform, 3, 0), get_element(transform, 3, 1), get_element(transform, 3, 2)]
    new_mins = translation.copy()
    new_maxs = translation.copy()

    for col in range(3):
        for row in range(3):
            a = get_element(transform, col, row) * mins[col]
            b = get_element(transform, col, row) * maxs[col]
            if a < b:
                new_mins[row] += a
                new_maxs[row] += b
            else:
                new_mins[row] += b
                new_maxs[row] += a

    new_mins_vec = vmath.Vector3(*new_mins)
    new_maxs_vec = vmath.Vector3(*new_maxs)
    return AABB(
        (new_mins_vec + new_maxs_vec) * 0.5,
        (new_maxs_vec - new_mins_vec) * 0.5
    )


def transform_aabb_minmax_from_book(transform, aabb):
    # note the swap
    mins_vec = aabb.center - aabb.extents
    maxs_vec = aabb.center + aabb.extents
    mins = [mins_vec.x, mins_vec.y, mins_vec.z]
    maxs = [maxs_vec.x, maxs_vec.y, maxs_vec.z]

    new_mins = [0.0, 0.0, 0.0]
    new_maxs = [0.0, 0.0, 0.0]

    for i in range(3):
        t = get_element(transform, 3, i)
        new_mins[i] = t
        new_maxs[i] = t
        for j in range(3):
            a = get_element(transform, j, i) * mins[j]  # m[i][j] -> m[j][i]
            b = get_element(transform, j, i) * maxs[j]
            if a < b:
                new_mins[i] += a
                new_maxs[i] += b
            else:
                new_mins[i] += b
                new_maxs[i] += a

    new_mins_vec = vmath.Vector3(*new_mins)
    new_maxs_vec = vmath.Vector3(*new_maxs)
    return AABB(
        (new_mins_vec + new_maxs_vec) * 0.5,
        (new_maxs_vec - new_mins_vec) * 0.5
    )


def points_to_aabb(points):
    float_max = 2 ** 20
    min_x = float_max
    min_y = float_max
    min_z = float_max
    max_x = -float_max
    max_y = -float_max
    max_z = -float_max
    for point in points:
        if point.x < min_x:
            min_x = point.x
        if point.y < min_y:
            min_y = point.y
        if point.z < min_z:
            min_z = point.z
        if point.x > max_x:
            max_x = point.x
        if point.y > max_y:
            max_y = point.y
        if point.z > max_z:
            max_z = point.z
    mins = vmath.Vector3(min_x, min_y, min_z)
    maxs = vmath.Vector3(max_x, max_y, max_z)
    return AABB(
        (mins + maxs) * 0.5,
        (maxs - mins) * 0.5
    )


def transform_aabb_naive(transform, aabb):
    center = aabb.center
    extents = aabb.extents
    mins = center - extents
    maxs = center + extents
    corners = [
        vmath.Vector3(mins.x, mins.y, mins.z),
        vmath.Vector3(mins.x, mins.y, maxs.z),
        vmath.Vector3(mins.x, maxs.y, mins.z),
        vmath.Vector3(mins.x, maxs.y, maxs.z),
        vmath.Vector3(maxs.x, mins.y, mins.z),
        vmath.Vector3(maxs.x, mins.y, maxs.z),
        vmath.Vector3(maxs.x, maxs.y, mins.z),
        vmath.Vector3(maxs.x, maxs.y, maxs.z),
    ]
    new_corners = []
    for corner in corners:
        new_corners.append(transform.transform_point(corner))
    return points_to_aabb(new_corners)


def aabb_to_obb(transform, aabb):
    axes = [
        vmath.Vector3(transform.m00, transform.m01, transform.m02),
        vmath.Vector3(transform.m10, transform.m11, transform.m12),
        vmath.Vector3(transform.m20, transform.m21, transform.m22),
    ]

    return OBB(transform.transform_point(aabb.center), axes, aabb.extents)


def plane_intersect_sphere(plane, sphere_position, sphere_radius):
    dot = plane.x * sphere_position.x + plane.y * sphere_position.y + plane.z * sphere_position.z + plane.w
    return dot > -sphere_radius


def planes_intersect_sphere(planes, sphere_position, sphere_radius):
    for plane in planes:
        if not plane_intersect_sphere(plane, sphere_position, sphere_radius):
            return False
    return True


def plane_intersect_aabb(plane, aabb):
    aabb_center = aabb.center
    aabb_extents = aabb.extents
    dot = plane.x * aabb_center.x + plane.y * aabb_center.y + plane.z * aabb_center.z + plane.w
    effective_radius = abs(aabb_extents.x * plane.x) + abs(aabb_extents.y * plane.y) + abs(aabb_extents.z * plane.z)
    return dot > -effective_radius


def planes_intersect_aabb(planes, aabb):
    for plane in planes:
        if not plane_intersect_aabb(plane, aabb):
            return False
    return True


def plane_intersect_obb(plane, obb):
    center = obb.center
    axes = obb.axes
    extents = obb.extents
    normal = vmath.Vector3(plane.x, plane.y, plane.z)
    dot = plane.x * center.x + plane.y * center.y + plane.z * center.z + plane.w
    effective_radius = abs(normal.dot(axes[0])) * extents.x + abs(normal.dot(axes[1])) * extents.y + abs(normal.dot(axes[2])) * extents.z
    return dot > -effective_radius


def planes_intersect_obb(planes, obb):
    for plane in planes:
        if not plane_intersect_obb(plane, obb):
            return False
    return True


def inverse_transform_plane(matrix, plane):
    m = matrix.transpose()
    x = vmath.Vector4(m.m00, m.m10, m.m20, m.m30).dot(plane)
    y = vmath.Vector4(m.m01, m.m11, m.m21, m.m31).dot(plane)
    z = vmath.Vector4(m.m02, m.m12, m.m22, m.m32).dot(plane)
    w = vmath.Vector4(m.m03, m.m13, m.m23, m.m33).dot(plane)
    length = math.sqrt(x * x + y * y + z * z)
    return vmath.Vector4(x, y, z, w) * (1.0 / length)


def normalize_plane(plane):
    normal = vmath.Vector3(plane.x, plane.y, plane.z)
    return plane * (1.0 / normal.length())


def view_projection_planes(matrix):
    m = matrix
    row0 = vmath.Vector4(m.m00, m.m10, m.m20, m.m30)
    row1 = vmath.Vector4(m.m01, m.m11, m.m21, m.m31)
    row2 = vmath.Vector4(m.m02, m.m12, m.m22, m.m32)
    row3 = vmath.Vector4(m.m03, m.m13, m.m23, m.m33)
    left = row0 + row3
    right = -row0 + row3
    bottom = row1 + row3
    top = -row1 + row3
    near = row2
    far = -row2 + row3

    return [
        normalize_plane(left),
        normalize_plane(right),
        normalize_plane(bottom),
        normalize_plane(top),
        normalize_plane(near),
        normalize_plane(far)
    ]


class Testculling(Unit):
    def __init__(self, world, unit_id, param):
        super().__init__(world, unit_id, param)

    def tick(self, delta_time):
        self._tick_culling(delta_time)
        self._tick_transform_aabb(delta_time)

    def _tick_culling(self, delta_time):
        game_time = self.world.game_time
        t = vutil.fract(game_time * 0.2)
        alpha = vutil.ping_pong(t)
        delta = alpha * 20.0 - 6.0

        aabb1 = AABB(vmath.Vector3(6.0, 0.0, delta), vmath.Vector3(3.0, 2.0, 1.0))
        aabb2 = AABB(vmath.Vector3(4.0, 0.0, 2.0 - delta), vmath.Vector3(2.0, 1.0, 1.5))

        aabbs = [aabb1, aabb2]

        sphere1 = (vmath.Vector3(0.0, 0.0, delta), 1.0)
        sphere2 = (vmath.Vector3(-5.0, 0.0, 4.0 - delta), 1.0)
        sphere3 = (vmath.Vector3(3.0, 0.0, 4.0 - delta), 1.0)
        sphere4 = (vmath.Vector3(delta, 0.0, 5.0), 1.0)

        spheres = [sphere1, sphere2, sphere3, sphere4]

        obb_rotation = vmath.Quaternion.from_angle_axis(game_time * 0.5, vutil.VEC3_Y)
        obb1 = OBB(vmath.Vector3(0.0, 0.0, delta), obb_rotation.to_matrix3(), vmath.Vector3(2, 1, 4))

        obbs = [obb1]


        planes = [
            vmath.Vector4(0.0, 0.0, 1.0, -1.0),
            vmath.Vector4(1.0, 0.0, 1.0, -3.0),
        ]

        transform = vmath.Transform()
        transform.translation = vmath.Vector3(math.cos(game_time) * 5, 0.0, math.sin(game_time * 0.5) * 2)
        transform.rotation = vmath.Quaternion.from_angle_axis(math.pi + game_time, vutil.VEC3_Y)

        fov = 1.0
        aspect = 1.0
        near = 1.0
        far = 10.0
        drawutil.draw_perspective(transform, fov, aspect, near, far)
        projection = vmath.Matrix4.from_perspective(fov, aspect, near, far)
        view_projection = projection * transform.to_matrix4().inverse()

        clip_planes = [
            vmath.Vector4(1.0, 0.0, 0.0, 1.0),
            vmath.Vector4(-1.0, 0.0, 0.0, 1.0),
            vmath.Vector4(0.0, 1.0, 0.0, 1.0),
            vmath.Vector4(0.0, -1.0, 0.0, 1.0),
            vmath.Vector4(0.0, 0.0, 1.0, 0.0),
            vmath.Vector4(0.0, 0.0, -1.0, 1.0)
        ]

        frustum_planes = []
        for plane in clip_planes:
            frustum_planes.append(inverse_transform_plane(view_projection, plane))

        planes = frustum_planes

        planes = view_projection_planes(view_projection)

        for aabb in aabbs:
            if planes_intersect_aabb(planes, aabb):
                color = vcolor.GREEN
            else:
                color = vcolor.BLACK
            drawutil.draw_aabb(aabb, color)

        for sphere in spheres:
            if planes_intersect_sphere(planes, *sphere):
                color = vcolor.GREEN
            else:
                color = vcolor.BLACK
            drawutil.draw_sphere(sphere[0], sphere[1], color)

        for obb in obbs:
            if planes_intersect_obb(planes, obb):
                color = vcolor.GREEN
            else:
                color = vcolor.BLACK
            drawutil.draw_obb(obb, color)

    def _tick_transform_aabb(self, delta_time):
        game_time = self.world.game_time
        # transform aabb
        aabb = AABB(vmath.Vector3(), vmath.Vector3(1.0, 2.0, 3.0))
        aabb_transform = vmath.Transform()
        aabb_transform.translation = vmath.Vector3(5.0, 0.0, 3.0)
        aabb_transform.rotation = vmath.Quaternion.from_euler_angles(vmath.Vector3(0.2, game_time * 0.1, game_time * 0.2))
        matrix = aabb_transform.to_matrix4()
        drawutil.draw_transform(aabb_transform)

        new_aabb = transform_aabb(matrix, aabb)
        new_aabb2 = transform_aabb_minmax(matrix, aabb)
        new_aabb3 = transform_aabb_naive(matrix, aabb)
        new_aabb4 = transform_aabb_from_book(matrix, aabb)
        new_aabb5 = transform_aabb_minmax_from_book(matrix, aabb)

        drawutil.draw_aabb(aabb, color=vcolor.MAGENTA)
        drawutil.draw_obb(aabb_to_obb(matrix, aabb), color=vcolor.BLUE)
        drawutil.draw_aabb(new_aabb, color=vcolor.BLACK)
        drawutil.draw_aabb(new_aabb2, color=vcolor.CYAN)
        drawutil.draw_aabb(new_aabb3, color=vcolor.GREEN)
        drawutil.draw_aabb(new_aabb4, color=vcolor.YELLOW)
        drawutil.draw_aabb(new_aabb5, color=vcolor.RED)
