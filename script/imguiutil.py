import ctypes

import imgui
import vmath
import toy
import toyentry


def address_to_bytes(address, size):
    return bytes((ctypes.c_ubyte * size).from_address(address))


class ImGuiRenderer:
    def __init__(self):
        self.io = imgui.get_io()
        self._font_texture = None

        self.io.delta_time = 1.0 / 60.0
        self.io.display_size = (500, 500)
        self._imgui_pipeline = toy.BasicPipeline.create_imgui()
        self.refresh_font_texture()

    def refresh_font_texture(self):
        width, height, pixels = self.io.fonts.get_tex_data_as_rgba32()
        self._font_texture = toy.Texture.create_rgba32(width, height, pixels)
        self._font_material = toy.Material.create(self._imgui_pipeline, self._font_texture)
        self.io.fonts.texture_id = self._font_material
        self.io.fonts.clear_tex_data()

    def render(self, draw_data):

        for commands in draw_data.commands_lists:
            idx_buffer_offset = 0

            vertex_data = address_to_bytes(commands.vtx_buffer_data, commands.vtx_buffer_size * imgui.VERTEX_SIZE)
            index_data = address_to_bytes(commands.idx_buffer_data, commands.idx_buffer_size * imgui.INDEX_SIZE)

            mesh = toy.ImGuiMesh.create(vertex_data, index_data)
            for command in commands.commands:
                texture_id = command.texture_id
                assert imgui.INDEX_SIZE == 4

                toy.app.imgui_manager.add_imgui_mesh_part(
                    mesh, vmath.Matrix4(), texture_id, command.elem_count, idx_buffer_offset)

                idx_buffer_offset += command.elem_count

    def shutdown(self):
        pass


_renderer = None

def startup():
    global _renderer
    imgui.create_context()
    _renderer = ImGuiRenderer()


_height = 10.0

def render():
    global _height
    imgui.new_frame()
    _height += 0.2
    if _height > 80.0:
        _height = 10.0  # TODO FIXME need scissor
    imgui.set_next_window_position(100, 50)
    imgui.set_next_window_size(300, _height, imgui.ALWAYS)  # imgui.ONCE
    imgui.begin('Custom window')
    imgui.text('Hello ImGui')
    imgui.text('Hello World')
    if imgui.button('Click'):
        print('clicked')
    imgui.end()

    imgui.render()
    _renderer.render(imgui.get_draw_data())
