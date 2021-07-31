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
