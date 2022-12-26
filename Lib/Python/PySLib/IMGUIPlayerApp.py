import OpenGL.GL as gl
import glfw, glumpy
from glumpy.app.window import window as GlumpyWindow
from glumpy.app.window.backends.backend_glfw import Window



__mouse_map__ = { glfw.MOUSE_BUTTON_LEFT:   GlumpyWindow.mouse.LEFT,
                      glfw.MOUSE_BUTTON_MIDDLE: GlumpyWindow.mouse.MIDDLE,
                      glfw.MOUSE_BUTTON_RIGHT:  GlumpyWindow.mouse.RIGHT }

__key_map__   = { glfw.KEY_ESCAPE:        GlumpyWindow.key.ESCAPE,
                  glfw.KEY_ENTER:         GlumpyWindow.key.ENTER,
                  glfw.KEY_TAB:           GlumpyWindow.key.TAB,
                  glfw.KEY_BACKSPACE:     GlumpyWindow.key.BACKSPACE,
                  glfw.KEY_INSERT:        GlumpyWindow.key.INSERT,
                  glfw.KEY_DELETE:        GlumpyWindow.key.DELETE,
                  glfw.KEY_RIGHT:         GlumpyWindow.key.RIGHT,
                  glfw.KEY_LEFT:          GlumpyWindow.key.LEFT,
                  glfw.KEY_DOWN:          GlumpyWindow.key.DOWN,
                  glfw.KEY_UP:            GlumpyWindow.key.UP,
                  glfw.KEY_PAGE_UP:       GlumpyWindow.key.PAGEUP,
                  glfw.KEY_PAGE_DOWN:     GlumpyWindow.key.PAGEDOWN,
                  glfw.KEY_HOME:          GlumpyWindow.key.HOME,
                  glfw.KEY_END:           GlumpyWindow.key.END,
                  glfw.KEY_CAPS_LOCK:     GlumpyWindow.key.CAPSLOCK,
                  glfw.KEY_LEFT_SHIFT:    GlumpyWindow.key.LSHIFT,
                  glfw.KEY_RIGHT_SHIFT:   GlumpyWindow.key.RSHIFT,
                  glfw.KEY_PRINT_SCREEN:  GlumpyWindow.key.PRINT,
                  glfw.KEY_PAUSE:         GlumpyWindow.key.PAUSE,
                  glfw.KEY_F1:            GlumpyWindow.key.F1,
                  glfw.KEY_F2:            GlumpyWindow.key.F2,
                  glfw.KEY_F3:            GlumpyWindow.key.F3,
                  glfw.KEY_F4:            GlumpyWindow.key.F4,
                  glfw.KEY_F5:            GlumpyWindow.key.F5,
                  glfw.KEY_F6:            GlumpyWindow.key.F6,
                  glfw.KEY_F7:            GlumpyWindow.key.F7,
                  glfw.KEY_F8:            GlumpyWindow.key.F8,
                  glfw.KEY_F9:            GlumpyWindow.key.F9,
                  glfw.KEY_F10:           GlumpyWindow.key.F10,
                  glfw.KEY_F11:           GlumpyWindow.key.F11,
                  glfw.KEY_F12:           GlumpyWindow.key.F12 }



# ------------------------------------------------------------------------------
# --------------------- abstract base class ------------------------------------
# ------------------------------------------------------------------------------

class ImGuiPlayerApp(glumpy.app.EventDispatcher):
    def __init__(self):
        super(ImGuiPlayerApp, self).__init__()

        d = self

        d.register_event_type('on_resize')

        d.register_event_type('on_enter')
        d.register_event_type('on_leave')

        d.register_event_type('on_key_press')
        d.register_event_type('on_key_release')

        d.register_event_type('on_character')

        d.register_event_type('on_mouse_press')
        d.register_event_type('on_mouse_release')
        d.register_event_type('on_mouse_drag')
        d.register_event_type('on_mouse_motion')
        d.register_event_type('on_mouse_scroll')

        self._mouse_x = 0
        self._mouse_y = 0

        self._button = GlumpyWindow.mouse.NONE



    def on_draw(self):
        pass

    def menu(self):
        pass



    _window = None

    @property
    def window(self):
        return self._window

    @window.setter
    def window(self, window):
        self._window = window

        x, y = glfw.get_cursor_pos(window)
        self._mouse_x = x
        self._mouse_y = y

        glfw.set_framebuffer_size_callback(self.window, self.on_framebuffer_resize)
        glfw.set_cursor_enter_callback(self.window, self.on_cursor_enter)
        glfw.set_char_callback(self.window, self.on_character)
        # The ones below are already called from IMGUIPlayer
        #glfw.set_key_callback(self.window, self.key_callback)
        #glfw.set_mouse_button_callback(self.window, self.mouse_button_callback)
        #glfw.set_cursor_pos_callback(self.window, self.mouse_cursor_pos_callback)
        #glfw.set_scroll_callback(self.window, self.mouse_scroll_callback)

    def on_framebuffer_resize(self, win, width, height):
        self.dispatch_event('on_resize', width, height)

    def on_cursor_enter(self, win, entered):
        if entered:
            self.dispatch_event('on_enter')
        else:
            self.dispatch_event('on_leave')

    def key_callback(self, win, key, scancode, action, mods):
        symbol = self._keyboard_translate(key)
        modifiers = self._modifiers_translate(mods)

        if action in[glfw.PRESS,glfw.REPEAT]:
            self.dispatch_event('on_key_press', symbol, modifiers)
        else:
            self.dispatch_event('on_key_release', symbol, modifiers)

    def on_character(self, win, character):
        #self.dispatch_event('on_character', u"%c" % character)
        pass


    def mouse_button_callback(self, win, button, action, mods):
        x,y = glfw.get_cursor_pos(win)
        #if self._hidpi:
        #    x, y = 2*x, 2*y

        button = __mouse_map__.get(button, GlumpyWindow.mouse.UNKNOWN)
        if action == glfw.RELEASE:
            self._button = GlumpyWindow.mouse.NONE
            self._mouse_x = x
            self._mouse_y = y
            self.dispatch_event('on_mouse_release', x, y, button)
        elif action == glfw.PRESS:
            self._button = button
            self._mouse_x = x
            self._mouse_y = y
            self.dispatch_event('on_mouse_press', x, y, button)

    def mouse_cursor_pos_callback(self, win, x, y):
        #if self._hidpi:
        #    x, y = 2*x, 2*y
        dx = x - self._mouse_x
        dy = y - self._mouse_y
        self._mouse_x = x
        self._mouse_y = y
        if self._button != GlumpyWindow.mouse.NONE:
            self.dispatch_event('on_mouse_drag', x, y, dx, dy, self._button)
        else:
            self.dispatch_event('on_mouse_motion', x, y, dx, dy)

    def mouse_scroll_callback(self, win, xoffset, yoffset):
        x,y = glfw.get_cursor_pos(win)
        #if self._hidpi:
        #    x, y = 2*x, 2*y
        self.dispatch_event('on_mouse_scroll', x, y, xoffset, yoffset)

    def window_size_callback(self, window, width, height):
        pass

    def _modifiers_translate(self, modifiers):
        _modifiers = 0
        if modifiers & glfw.MOD_SHIFT:
            _modifiers |= GlumpyWindow.key.MOD_SHIFT
        if modifiers & glfw.MOD_CONTROL:
            _modifiers |= GlumpyWindow.key.MOD_CTRL
        if modifiers & glfw.MOD_ALT:
            _modifiers |= GlumpyWindow.key.MOD_ALT
        if modifiers & glfw.MOD_SUPER:
            _modifiers |= GlumpyWindow.key.MOD_COMMAND
        self._modifiers = modifiers
        return _modifiers

    def _keyboard_translate(self, code):
        if (32 <= code <= 96) or code in [161, 162]:
            return code
        return __key_map__.get(code, GlumpyWindow.key.UNKNOWN)