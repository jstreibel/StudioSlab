# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import glfw
from glumpy import app

from Simulation.Simulation import Simulation
from ViewControl.Visualization import Visualization, Console

import imgui
from imgui.integrations.glumpy import GlumpyRenderer


SAMPLES                = 0

ViewportWidth          = 1800
ViewportHeight         = int(1800*(9./16))

GridWidth              = 2**6
SpaceWidth             = 1

r = 0.6e-2  # dt/h = r

GridHeight             = GridWidth
SpaceHeight            = SpaceWidth


h = SpaceWidth/GridWidth
dt = r*h


#config = app.configuration.Configuration()
#config.samples = SAMPLES
#config.profile = "core"
#config.vsync = True
# _config.major_version = 4
# _config.minor_version = 6

imgui.create_context()
__imgui_io__ = imgui.get_io()

app.use(backend='glfw_imgui', api='GL', profile='core')
__window__ = app.Window(ViewportWidth, ViewportHeight, color=(0.54, 0.60, 0.98, 1))
print("Backend:", __window__._backend)
print("Window:", __window__)

#__imgui_renderer__ = GlumpyRenderer(__window__, attach_callbacks=False)


__viewer__ = None
__sim__ = None


runSim = False
nStepsPerRenderFrame: int = 150


@__window__.event
def on_draw(frame_dt):
    if runSim:
        __sim__.stepSim(dt, nStepsPerRenderFrame)

    __viewer__.draw(__sim__)

    #imgui.begin("Demo")
    #imgui.button("Hello!")
    #imgui.end()



    #imgui.render()
    #__imgui_renderer__.render(imgui.get_draw_data())

    #ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


@__window__.event
def on_key_press(symbol, modifiers):
    global nStepsPerRenderFrame, runSim

    if symbol == 32:
        runSim = not runSim
    elif symbol == 65365:  # page up
        nStepsPerRenderFrame += 10
    elif symbol == 65366:  # page down
        if(nStepsPerRenderFrame >= 10):
            nStepsPerRenderFrame -= 10
    elif symbol == 65289:  # tab
        pass
    else:
        __viewer__.on_key_press(symbol, modifiers)


@__window__.event
def on_mouse_drag(x, y, dx, dy, button):
    pass


@__window__.event
def on_resize(w, h):
    global ViewportWidth, ViewportHeight
    ViewportWidth, ViewportHeight = w, h

    __viewer__.resize(w, h)


@__window__.event
def on_init():
    global __window__, __viewer__, __sim__, __imgui_io__

    __viewer__ = Visualization(__window__, GridDim=(GridWidth, GridHeight), SpaceDim=(SpaceWidth, SpaceHeight))
    __sim__ = Simulation(dt, GridDim=(GridWidth,GridHeight), SpaceDim=(SpaceWidth,SpaceHeight))
    __sim__.applyInitConditions()


if __name__ == '__main__':
    app.run()