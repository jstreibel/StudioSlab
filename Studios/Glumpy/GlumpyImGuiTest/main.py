from glumpy import app
from imgui.integrations.glfw import GlfwRenderer
import imgui
import glfw.GLFW as glfw

app.use("pyglfw")
window = app.Window(800, 600)

imgui.create_context()    # line 1
imguiRenderer = GlfwRenderer(window._native_window)    # line 2

#@window.event
#def on_draw():


app.run()
