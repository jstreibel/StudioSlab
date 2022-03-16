import imgui
from glumpy import app
from glumpy.app import Viewport as GLViewport

from PySLib.Viewers2D import Viewer3D


WindowWidth = int(1920*1.5)
WindowHeight = int(1080*1.5)


app.use("glfw_imgui")  # Required for ImGui integration
window = app.Window(width=WindowWidth, height=WindowHeight,
                    color=(0.30, 0.30, 0.35, 1.00))


vp = GLViewport((WindowWidth, WindowHeight))
window.attach(vp)


def pressure(_T, _vol):
    T = (_T+.6)*10
    vol = (_vol+.6)*10

    a=1
    b=1

    return (_T, _vol, 1.e-3*(T/(vol-b) - a/vol**2))

viewer3D = Viewer3D(viewport=vp, surface=pressure)


@window.event
def on_draw(frame_dt):
    window.clear()

    viewer3D.draw()

    imgui.new_frame()
    viewer3D.draw_menu()
    imgui.end_frame()
    imgui.render()


app.run()