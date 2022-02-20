import ctypes

import imgui
import vmath
import toy
import keycodes


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

        self.io.get_clipboard_text_fn = self._get_clipboard_text
        self.io.set_clipboard_text_fn = self._set_clipboard_text
        self._map_keys()

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
                    mesh, vmath.Matrix4(), texture_id, command.elem_count, idx_buffer_offset,
                    vmath.Vector4(*command.clip_rect)
                )

                idx_buffer_offset += command.elem_count

    def shutdown(self):
        pass

    def _get_clipboard_text(self):
        return toy.app.input_manager.get_clipboard_text()

    def _set_clipboard_text(self, text):
        toy.app.input_manager.set_clipboard_text(text)

    def _map_keys(self):
        key_map = self.io.key_map

        key_map[imgui.KEY_TAB] = keycodes.VK_TAB
        key_map[imgui.KEY_LEFT_ARROW] = keycodes.VK_LEFT
        key_map[imgui.KEY_RIGHT_ARROW] = keycodes.VK_RIGHT
        key_map[imgui.KEY_UP_ARROW] = keycodes.VK_UP
        key_map[imgui.KEY_DOWN_ARROW] = keycodes.VK_DOWN
        key_map[imgui.KEY_PAGE_UP] = keycodes.VK_PRIOR
        key_map[imgui.KEY_PAGE_DOWN] = keycodes.VK_NEXT
        key_map[imgui.KEY_HOME] = keycodes.VK_HOME
        key_map[imgui.KEY_END] = keycodes.VK_END
        key_map[imgui.KEY_DELETE] = keycodes.VK_DELETE
        key_map[imgui.KEY_BACKSPACE] = keycodes.VK_BACK
        key_map[imgui.KEY_ENTER] = keycodes.VK_RETURN
        key_map[imgui.KEY_ESCAPE] = keycodes.VK_ESCAPE
        key_map[imgui.KEY_A] = keycodes.VK_A
        key_map[imgui.KEY_C] = keycodes.VK_C
        key_map[imgui.KEY_V] = keycodes.VK_V
        key_map[imgui.KEY_X] = keycodes.VK_X
        key_map[imgui.KEY_Y] = keycodes.VK_Y
        key_map[imgui.KEY_Z] = keycodes.VK_Z

    def process_inputs(self):
        io = self.io
        io_keys_down = io.keys_down
        input_manager = toy.app.input_manager

        io.display_size = toy.app.get_window().get_window_size()
        io.delta_time = toy.app.delta_time

        io.mouse_pos = input_manager.get_mouse_position()
        io.mouse_down[0] = input_manager.get_mouse_button(0)
        io.mouse_down[1] = input_manager.get_mouse_button(1)
        io.mouse_down[2] = input_manager.get_mouse_button(2)
        io.mouse_wheel = input_manager.get_mouse_wheel()

        for char in input_manager.get_chars():
            if 0 < char < 0x10000:
                io.add_input_character(char)

        keys_down = input_manager.get_keys_down()
        keys_up = input_manager.get_keys_up()
        for key in keys_down:
            io_keys_down[key] = True
        for key in keys_up:
            io_keys_down[key] = False

        io.key_ctrl = io_keys_down[keycodes.VK_CONTROL]
        io.key_alt = io_keys_down[keycodes.VK_MENU]  # not work
        io.key_shift = io_keys_down[keycodes.VK_SHIFT]
        io.key_super = io_keys_down[keycodes.VK_LWIN] or io_keys_down[keycodes.VK_RWIN]


_renderer = None


def want_capture_keyboard():
    return imgui.get_io().want_capture_keyboard


def startup():
    global _renderer
    imgui.create_context()
    toy.app.want_capture_keyboard = want_capture_keyboard
    _renderer = ImGuiRenderer()



def render():
    _renderer.process_inputs()
    imgui.new_frame()
    imgui.set_next_window_size(300, 200, imgui.ONCE)
    imgui.begin('Custom window')
    imgui.text('Hello ImGui')
    imgui.text('Hello World')
    if imgui.button('Click'):
        print('clicked')
    changed, text_val = imgui.input_text('text', 'type here', 100)
    imgui.slider_float('float', 0.5, 0.0, 1.0)
    imgui.end()

    imgui.render()
    _renderer.render(imgui.get_draw_data())
