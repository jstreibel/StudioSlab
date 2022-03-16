import sys
import numpy as np

from glumpy import gl
import glfw

import imgui
from imgui.integrations.glfw import GlfwRenderer

from IMGUIPlayerApp import ImGuiPlayerApp

# ------------------------------------------------------------------------------
# ------------------------ generic player class  -------------------------------
# ------------------------------------------------------------------------------

class ImGuiPlayer:
    # ---------------------------------------------------------------------------
    # --------------------------- class variables -------------------------------
    # ---------------------------------------------------------------------------

    window = None
    impl: GlfwRenderer = None

    app: ImGuiPlayerApp = None

    quit_requested: bool = None

    def __init__(self):
        # initialize window and GUI
        imgui.create_context()

        self.window = self.impl_glfw_init()
        #glfw.set_window_monitor(self.window, glfw.get_primary_monitor(), 0,0, 3840, 2160, 60)  # FULLSCREEN
        self.impl = GlfwRenderer(self.window)

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
        #glfw.window_hint(glfw.OPENGL_FORWARD_COMPAT, gl.GL_TRUE)

        window = glfw.create_window(int(width), int(height), window_name, None, None)
        glfw.make_context_current(window)

        if not window:
            glfw.terminate()
            print("Could not initialize Window")
            sys.exit(1)

        return window

    # ---------------------------------------------------------------------------
    # --------------------------- class methods - gui and menu   ----------------
    # ---------------------------------------------------------------------------

    def key_callback(self, window, key, scancode, action, mods):

        io = imgui.get_io()
        self.impl.keyboard_callback(window, key, scancode, action, mods)

        if io.want_capture_keyboard:
            #print("imgui handles")
            pass
        else:
            # print("processed by app: key pressed")

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

        if io.want_capture_mouse:
            #print("imgui handles")
            pass
        else:
            self.app.mouse_button_callback(window, button, action, mods)

    def mouse_scroll_callback(self, window, xoffset, yoffset):
        io = imgui.get_io();

        if io.want_capture_mouse:
            #print("imgui handles")
            pass
        else:
            # print( "processed by app: scroll: {},{}".format( xoffset, yoffset ));

            self.app.mouse_scroll_callback(window, xoffset, yoffset)

    def window_size_callback(self, window, width, height):
        self.app.window_size_callback(window, width, height)

    def run(self, app: ImGuiPlayerApp):
        self.app = app
        self.app.window_size_callback(self.window, 1200, 1200)


        glfw.set_key_callback(self.window, self.key_callback)
        glfw.set_cursor_pos_callback(self.window, self.mouse_cursor_pos_callback)
        glfw.set_mouse_button_callback(self.window, self.mouse_button_callback)
        glfw.set_scroll_callback(self.window, self.mouse_scroll_callback)
        glfw.set_window_size_callback(self.window, self.window_size_callback)

        while not glfw.window_should_close(self.window) and not self.quit_requested:
            glfw.poll_events()
            self.impl.process_inputs()

            self.app.menu()

            gl.glClearColor(0., 0., 0.2, 1)
            gl.glClear(gl.GL_COLOR_BUFFER_BIT)

            self.app.on_draw()

            imgui.render()
            self.impl.render(imgui.get_draw_data())
            glfw.swap_buffers(self.window)

        #self.impl.shutdown()
        glfw.terminate()
