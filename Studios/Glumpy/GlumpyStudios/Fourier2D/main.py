# Press Shift+F10 to execute it or replace it with your code.
# Press Double Shift to search everywhere for classes, files, tool windows, actions, and settings.
import numpy
from glumpy import app

from ViewTools import Console
from Simulation import Simulation
from Visualization import Visualization


SAMPLES                = 1

ViewportWidth          = 1920
ViewportHeight         = 1080

GridWidth              = 2**9
SpaceWidth             = 300.0
SpaceHeight            = 0.3333*SpaceWidth
uvCountX               = 2**10

DiracDelta_a = 0.5
DiracDelta_eps = 1

VisualScalePhi         = 1e-1
VisualScaledPhidt      = 1e0

FourierMapScale        = 2*numpy.pi  # is a unit

r = 1e-3  # dt/h = r

GridHeight             = GridWidth
uvCountY               = int(uvCountX*(SpaceHeight/SpaceWidth))
VizUVCount             = uvCountX, uvCountY



h = SpaceWidth/GridWidth
dt = r*h


_config = app.configuration.Configuration()
_config.samples = SAMPLES
_config.profile = "core"
#config.major_version = 4
#config.minor_version = 6

window = app.Window(ViewportWidth, ViewportHeight, color=(0.48,0.56,0.94,1), config=_config, fullscreen=True)


__viewer__ = None
__console__ = None
__sim__ = None


runSim = True
nStepsPerRenderFrame: int = 150


@window.event
def on_draw(frame_dt):
    if runSim:
        __sim__.stepSim(dt, nStepsPerRenderFrame)

    __viewer__.clearConsole()
    Print = __viewer__.write
    Print("Grid {}x{}".format(GridWidth, GridHeight))
    Print("{} steps/frame".format(nStepsPerRenderFrame))
    Print("k0={}".format(round(__sim__.k0, 3)))
    Print("th0={}".format(round(__sim__.th0,3)))

    __viewer__.draw(window, frame_dt, __sim__)

    __console__.draw()


@window.event
def on_key_press(symbol, modifiers):
    global nStepsPerRenderFrame, runSim

    da = 0.025  # delta ang

    if symbol == 32:
        runSim = not runSim
        if runSim:
            Print("Simulation is running")
        else:
            Print("Simulation has stopped")
    elif symbol == 65365:  # page up
        nStepsPerRenderFrame += 10
        Print("Stepping %i per frame." % nStepsPerRenderFrame)
    elif symbol == 65366:  # page down
        if(nStepsPerRenderFrame >= 10):
            nStepsPerRenderFrame -= 10
        Print("Stepping %i per frame." % nStepsPerRenderFrame)
    elif symbol == 65289:  # tab
        __console__.toggleVisibility()
        pass
    elif symbol == 65361:  # esq
        __sim__.deltak0(-da)
    elif symbol == 65362:  # up
        __sim__.deltaTh0(+da)
    elif symbol == 65363:  # dir
        __sim__.deltak0(+da)
    elif symbol == 65364:  # down
        __sim__.deltaTh0(-da)
    elif symbol == 65470:  # F1
        on = __viewer__.toggleHelp()
        if on:
            Print("Help on")
        else:
            Print("Help off")
    elif symbol == 61:  # +
        subdivs = __viewer__.increaseGridSubdivs()
        Print("Increasing grid subdivisions to %i"%subdivs)
    elif symbol == 45:  # -
        subdivs = __viewer__.decreaseGridSubdivs()
        Print("Decreasing grid subdivisions to %i" % subdivs)
    elif symbol == 49:  # 1
        Print("1")
    elif symbol == 50:  # 2
        Print("2")
    elif symbol == 70:  # f
        __viewer__.swapFieldVisualization()
        Print("Swapping phi<->dPhidt")
    elif symbol == 71:  # g
        __viewer__.cycleGridType()
        Print("Cycling grids.")
    elif symbol == 76:  # l
        __viewer__.cycleLighting()
        Print("Cycling lighting.")
    elif symbol == 86:  # v
        __viewer__.toggleLevelLines()
        Print("Toggle level lines")
    elif symbol == 93:  # ]
        __sim__.deltaEps( da*0.2)
    elif symbol == 91:  # [
        __sim__.deltaEps(-da*0.2)
    else:
        Print("Unknown command. Symbol: " + str(symbol) + ". Mods: " + str(modifiers))


@window.event
def on_resize(w, h):
    global ViewportWidth, ViewportHeight
    ViewportWidth, ViewportHeight = w, h

    __viewer__.resize(w, h)


@window.event
def on_init():
    global __viewer__, __sim__
    __viewer__ = Visualization(window, GridDim=(GridWidth, GridHeight), SpaceDim=(SpaceWidth,SpaceHeight),
                               uvCount=VizUVCount, phiScale=VisualScalePhi, dPhidtScale=VisualScaledPhidt, FourierMapScale=FourierMapScale)
    __sim__ = Simulation(dt, GridDim=(GridWidth,GridHeight), SpaceDim=(SpaceWidth,SpaceHeight),
                         FourierMapScale=FourierMapScale)

    FixedPrint = __viewer__.addHelpMessage
    FixedPrint("Commands")
    FixedPrint("F1: show this help")
    FixedPrint("Space: toggle run sim")
    FixedPrint("PgUp: steps/frame += 10")
    FixedPrint("PgDn: steps/frame -= 10")
    FixedPrint("Tab: toggle console")
    FixedPrint("g: cycle grid types")
    FixedPrint("l: cycle lighting")
    FixedPrint("v: toggle level lines")
    FixedPrint("+/-: change grid subdivs.")
    FixedPrint("f: change field visualization.")



__console__ = Console.Console(scale=2, no_events=True, x=1, y=10, cols=40,
                              fg_color=(0,0,0,1), bg_color=(0,0,0,.2), visible=False)
window.attach(__console__)

Print = __console__.write

if __name__ == '__main__':
    app.run()
