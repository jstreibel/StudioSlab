import sys

import glumpy.app
import numpy as np

from glumpy import gl, app
import glfw

import imgui
from imgui.integrations.glfw import GlfwRenderer

from IMGUIPlayerApp import ImGuiPlayerApp


class ImGuiPlayer:
    window = None
    impl: GlfwRenderer = None
    #impl: GlumpyRenderer

    app: ImGuiPlayerApp = None

    quit_requested: bool = None


    def __init__(self, fullscreen=True):
        # initialize window and GUI
        imgui.create_context()

        self.window = self.impl_glfw_init()
        if fullscreen:
            glfw.set_window_monitor(self.window, glfw.get_primary_monitor(), 0,0, 3840, 2160, 60)  # FULLSCREEN
        self.impl = GlfwRenderer(self.window)

        glfw.set_key_callback(self.window, self.key_callback)
        glfw.set_cursor_pos_callback(self.window, self.mouse_cursor_pos_callback)
        glfw.set_mouse_button_callback(self.window, self.mouse_button_callback)
        glfw.set_scroll_callback(self.window, self.mouse_scroll_callback)
        glfw.set_window_size_callback(self.window, self.window_size_callback)

        self.quit_requested = False


    def impl_glfw_init(self):
        width, height = 1920, 1080
        window_name = "Studies"

        if not glfw.init():
            print("Could not initialize OpenGL context")
            exit(1)

        glfw.window_hint(glfw.CONTEXT_VERSION_MAJOR, 4)
        glfw.window_hint(glfw.CONTEXT_VERSION_MINOR, 6)

        # the OPENGL_COMPAT_PROFILE enables the mix between pyimgui and glumpy
        glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_COMPAT_PROFILE)
        # glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)
        # glfw.window_hint(glfw.OPENGL_PROFILE, glfw.OPENGL_ANY_PROFILE)
        # glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, gl.GL_TRUE)

        window = glfw.create_window(int(width), int(height), window_name, None, None)
        glfw.make_context_current(window)

        if not window:
            glfw.terminate()
            print("Could not initialize Window")
            sys.exit(1)

        return window


    # ******************************************************************************************
    # *** CALLBACKS ****************************************************************************

    def key_callback(self, window, key, scancode, action, mods):

        io = imgui.get_io()
        self.impl.keyboard_callback(window, key, scancode, action, mods)

        if not io.want_capture_keyboard:
            if key == glfw.KEY_ESCAPE and action == glfw.RELEASE:
                self.quit_requested = True
            if key == glfw.KEY_Q and np.bitwise_and(mods, glfw.MOD_CONTROL):
                self.quit_requested = True

            self.app.key_callback(window, key, scancode, action, mods)

    def mouse_cursor_pos_callback(self, window, xpos, ypos):
        io = imgui.get_io()

        if not io.want_capture_mouse:
            self.app.mouse_cursor_pos_callback(window, xpos, ypos)

    def mouse_button_callback(self, window, button, action, mods):
        io = imgui.get_io()

        if not io.want_capture_mouse:
            self.app.mouse_button_callback(window, button, action, mods)

    def mouse_scroll_callback(self, window, xoffset, yoffset):
        io = imgui.get_io()

        if not io.want_capture_mouse:
            self.app.mouse_scroll_callback(window, xoffset, yoffset)

    def window_size_callback(self, window, width, height):
        self.app.window_size_callback(window, width, height)

    # *** END CALLBACKS ************************************************************************
    # ******************************************************************************************


    def run(self, app: ImGuiPlayerApp):
        self.app = app
        app.window = self.window

        while not glfw.window_should_close(self.window) and not self.quit_requested:
            self._extracted_from_run_7()
        #self.impl.shutdown()
        glfw.terminate()

    def _extracted_from_run_7(self):
        glfw.poll_events()
        self.impl.process_inputs()

        self.app.menu()

        gl.glClearColor(0.2, 0.2, 0.5, 1)
        #gl.glClear(gl.GL_COLOR_BUFFER_BIT)

        self.app.on_draw()

        imgui.render()
        self.impl.render(imgui.get_draw_data())
        glfw.swap_buffers(self.window)
