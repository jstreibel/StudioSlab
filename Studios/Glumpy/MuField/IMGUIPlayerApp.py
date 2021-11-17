import OpenGL.GL as gl


# ------------------------------------------------------------------------------
# --------------------- abstract base class ------------------------------------
# ------------------------------------------------------------------------------
class ImGuiPlayerApp:

    def __init__(self):
        pass

    def initialize_program(self):
        pass

    def key_callback(self, window, key, scancode, action, mods):
        pass

    def mouse_cursor_pos_callback(self, window, xpos, ypos):
        pass

    def mouse_button_callback(self, window, button, action, mods):
        pass

    def mouse_scroll_callback(self, window, xoffset, yoffset):
        pass

    def window_size_callback(self, window, width, height):
        gl.glViewport(0, 0, width, height)

    def on_draw(self):
        pass

    def menu(self):
        pass
