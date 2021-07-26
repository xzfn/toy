
import os
import urllib

import vmath
import toy

import pygltflib


def load_gltf(filename):
    return GLTFLoader(filename).build()


def read_uri(uri, path=None):
    if ':' not in uri:
        filedir = path
        fileuri = os.path.join(filedir, uri)
        with open(fileuri, 'rb') as f:
            return f.read()
    else:
        with urllib.request.urlopen(uri) as f:
            return f.read()


class GLTFLoader:
    def __init__(self, filename):
        self.filename = filename
        self.gltf = pygltflib.GLTF2().load(filename)

        self._uris = {}
        self._buffers = {}
        self._buffer_views = {}

    def get_uri(self, uri):
        if uri in self._uris:
            return self._uris[uri]
        data = read_uri(uri, self.gltf._path)
        self._uris[uri] = data
        return data

    def get_buffer(self, buffer_index):
        if buffer_index in self._buffers:
            return self._buffers[buffer_index]
        buffer = self.gltf.buffers[buffer_index]
        data = self.get_uri(buffer.uri)
        assert len(data) == buffer.byteLength
        self._buffers[buffer_index] = data
        return data

    def get_buffer_view(self, buffer_view_index):
        if buffer_view_index in self._buffer_views:
            return self._buffer_views[buffer_view_index]
        buffer_view = self.gltf.bufferViews[buffer_view_index]
        data = self.get_buffer(buffer_view.buffer)
        byte_offset = buffer_view.byteOffset
        byte_length = buffer_view.byteLength
        view = memoryview(data)[byte_offset : byte_offset + byte_length]
        self._buffer_views[buffer_view_index] = view
        return view

    def get_accessor(self, accessor_index):
        accessor = self.gltf.accessors[accessor_index]
        byte_offset = accessor.byteOffset
        buffer_view = accessor.bufferView
        buffer_view_data = self.get_buffer_view(buffer_view)[byte_offset:]

        type_code = ''
        component_type = accessor.componentType
        if component_type == 5126:
            type_code = 'f'
        elif component_type == 5123:
            type_code = 'H'
        elif component_type == 5125:
            type_code = 'I'
        else:
            raise RuntimeError('bad type code')
        buffer_view_values = buffer_view_data.cast(type_code)

        count = accessor.count
        type_ = accessor.type
        if type_ == 'SCALAR':
            return [buffer_view_values[i] for i in range(count)]
        elif type_ == 'VEC2':
            vec_list = []
            for i in range(count):
                byte_index = i * 2
                vec_list.append(vmath.Vector2(
                    buffer_view_values[byte_index],
                    buffer_view_values[byte_index + 1]))
            return vec_list
        elif type_ == 'VEC3':
            vec_list = []
            for i in range(count):
                byte_index = i * 3
                vec_list.append(vmath.Vector3(
                    buffer_view_values[byte_index],
                    buffer_view_values[byte_index + 1],
                    buffer_view_values[byte_index + 2]))
            return vec_list
        elif type_ == 'VEC4':
            vec_list = []
            for i in range(count):
                byte_index = i * 3
                vec_list.append(vmath.Vector4(
                    buffer_view_values[byte_index],
                    buffer_view_values[byte_index + 1],
                    buffer_view_values[byte_index + 2],
                    buffer_view_values[byte_index + 3]))
            return vec_list
        else:
            raise RuntimeError('bad accessor type')

    def build_primitive(self, primitive):
        attributes = primitive.attributes
        indices_index = primitive.indices
        positions = self.get_accessor(attributes.POSITION)
        normals = self.get_accessor(attributes.NORMAL)
        uvs = self.get_accessor(attributes.TEXCOORD_0)
        mesh_data = toy.MeshData()
        mesh_data.set_vertices_data(positions, normals, uvs)
        mesh_data.set_indices(self.get_accessor(indices_index))
        return mesh_data

    def build(self):
        primitive_objs = []
        for mesh in self.gltf.meshes:
            primitives = mesh.primitives
            primitive = primitives[0]
            primitive_objs.append(self.build_primitive(primitive))
        return primitive_objs
