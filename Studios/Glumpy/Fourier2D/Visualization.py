import numpy

import Simulation
from ViewTools.SurfaceSlab import ShaderDir
from ViewTools import Console

from glumpy import gl, gloo, glm
from glumpy.geometry import surface as ParametricSurface
from glumpy.transforms import TrackballPan
from glumpy.graphics.text import FontManager
from glumpy.graphics.collections import GlyphCollection
from glumpy.transforms import Position

from numpy import asarray, eye, float32, array, matrix, dot

import time


class TimerError(Exception):
    """A custom exception used to report errors in use of Timer class"""


class Timer:
    def __init__(self):
        self._start_time = None

    def start(self):
        """Start a new timer"""
        if self._start_time is not None:
            raise TimerError(f"Timer is running. Use .stop() to stop it")

        self._start_time = time.perf_counter()

    def getElapsed(self):
        return time.perf_counter() - self._start_time

    def stop(self):
        """Stop the timer, and report the elapsed time"""
        if self._start_time is None:
            raise TimerError(f"Timer is not running. Use .start() to start it")

        elapsed_time = time.perf_counter() - self._start_time
        self._start_time = None
        #print(f"Elapsed time: {elapsed_time:0.4f} seconds")


class Visualization(object):
    __totalLightModes = 3
    __LIGHTMODE_LIGHTOFF = 0
    __LIGHTMODE_LIGHTON_WITHCOLORS = 1
    __LIGHTMODE_LIGHTON_WHITE = 2

    def __init__(self, window, GridDim: (float,float), SpaceDim,
                 phiScale: float=1, dPhidtScale: float=1, uvCount=(256,256), FourierMapScale: float = 2*numpy.pi) -> None:
        super().__init__()

        self._SpaceDim = SpaceDim
        self._GridDim = GridDim

        self._initProgram(window, GridDim, SpaceDim, phiScale, dPhidtScale, uvCount, FourierMapScale)

        color_val=.15
        self._console = Console.Console(scale=2, no_events=True, cols=40, rows=8,
                                        x=1, y=1,
                                        fg_color=(color_val,color_val,color_val,1), bg_color=(0,0,0,0))
        window.attach(self._console)

        self._timer = Timer()
        self._timer.start()

        self._showHelp = False


    def _initProgram(self, window, GridDim: (float,float), SpaceDim, phiScale: float, dPhidtScale: float, uvCount, FourierMapScale) -> None:
        SpaceWidth, SpaceHeight = SpaceDim
        GridWidth, GridHeight = GridDim


        trackball = TrackballPan(Position("v_position"), znear=0, nfar=50, distance=5)

        view = eye(4, dtype=float32)
        model = eye(4, dtype=float32)
        projection = eye(4, dtype=float32)
        glm.translate(view, 0, 0, -2)
        normal = array(matrix(dot(view, model)).I.T)


        umin, umax, vmin, vmax = -SpaceWidth * .5, SpaceWidth * .5, -SpaceHeight * .5, SpaceHeight * .5


        v = 0.1
        winW, winH = window.width, window.height
        ratio = winW/winH
        self._plane = ParametricSurface(lambda u, v: asarray((u+0.81, v*ratio+0.68, 0)), umin=-v, umax=v, vmin=-v, vmax=v)
        self._planeProg = gloo.Program(ShaderDir + "PlaneShading.vert", ShaderDir + "PlaneShading.frag")
        self._planeProg.bind(self._plane[0])
        self._planeProg['S'] = FourierMapScale


        #func = lambda u,v: 2.2*exp(-(u**2 + v**2)/0.1**2)
        func = lambda u, v: 0
        self._vertices, self._indices = ParametricSurface(lambda u, v: asarray((u, v, func(u, v)))
                                                          , urepeat=1, umin=umin, umax=umax, ucount=uvCount[0]
                                                          , vrepeat=1, vmin=vmin, vmax=vmax, vcount=uvCount[1])


        self._prog_visualize = gloo.Program(ShaderDir + "FieldShading.vert", ShaderDir + "FieldShading.frag")
        self._prog_visualize.bind(self._vertices)

        self._prog_visualize['model'] = model
        self._prog_visualize['view'] = view
        self._prog_visualize['normal'] = normal

        self._currentLightMode = self.__LIGHTMODE_LIGHTON_WITHCOLORS  # quando chamar self.cycleLighting() isso vai avancar
        self.cycleLighting()
        self._showLevelLines = False
        self._prog_visualize['showLevelLines'] = self._showLevelLines

        a = 2
        b = 1-a
        self._prog_visualize['light1_color'] = a, b, 0
        self._prog_visualize['light2_color'] = 0, a, b
        self._prog_visualize['light3_color'] = b, 0, a

        self._gridType = 0
        self._gridSubdivs = 4
        self._prog_visualize['gridType'] = self._gridType
        self._prog_visualize['gridSubdivs'] = self._gridSubdivs

        self._viewMode = 0
        #self._prog_visualize['viewMode'] = self._viewMode

        self._view = view
        self._angs = [360, 530]

        self._prog_visualize["phiScale"] = phiScale
        self._prog_visualize["dPhidtScale"] = dPhidtScale
        self._prog_visualize['dr'] = SpaceWidth / GridWidth, SpaceHeight / GridHeight

        self._prog_visualize['S'] = FourierMapScale

        self._prog_visualize['t'] = 0

        self._prog_visualize['transform'] = trackball

        window.attach(self._prog_visualize['transform'])


        #trackball.aspect = 10
        trackball.zoom = 60
        trackball.distance = 30
        self._trackaball = trackball


        self.__initLabels(window, SpaceDim)

    def __initLabels(self, window, SpaceDim):
        labels = GlyphCollection('agg')

        x, y, z = 0.95, -0.25, 0
        font_size = 16

        font = FontManager.get("OpenSans-Regular.ttf", size=font_size, mode='agg')

        window.attach(labels["transform"])
        window.attach(labels["viewport"])

        self._helpFont = font
        self._helpLabels = labels
        self._helpLabelsOrigin = (x,y,z)

        size = window._width, window._height
        self.__helpLabelsLineSpacing = .8/font_size

    def addHelpMessage(self, msg: str):
        font, labels, (x,y,z) = self._helpFont, self._helpLabels, self._helpLabelsOrigin

        y -= self.__helpLabelsLineSpacing
        self._helpLabels.append(msg, font, origin=(x,y,z), anchor_x='right', anchor_y='top')

        self._helpLabelsOrigin = (x,y,x)

    def clearConsole(self):
        self._console.clear()

    def write(self, text: str, style=None):
        if text is not str:
            text = str(text)
        self._console.write(text, style)

    def cycleGridType(self):
        self._gridType = (self._gridType+1) % 3
        self._prog_visualize['gridType'] = self._gridType

    def draw(self, window, frame_dt:float, simulation: Simulation.Simulation):
        gl.glViewport(0, 0, window.width, window.height)
        #gl.glClearColor(0, 0, 0, 1)
        #gl.glClear(gl.GL_COLOR_BUFFER_BIT)
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_BLEND)
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)

        window.clear()

        self._prog_visualize['field'] = simulation.PhiValue
        #self._prog_visualize['field'].wrapping = gl.GL_CLAMP_TO_BORDER
        self._prog_visualize['field'].wrapping = gl.GL_REPEAT
        self._prog_visualize['field'].interpolation = gl.GL_LINEAR
        #self._prog_visualize['field'].interpolation = gl.GL_NEAREST

        #self._angs[0] += .5
        #self._angs[1] += .4
        theta = self._angs[0]
        phi = self._angs[1]
        model = eye(4, dtype=float32)
        glm.rotate(model, theta, 0, 0, 1)
        glm.rotate(model, phi, 0, 1, 0)
        self._prog_visualize['normal'] = array(matrix(dot(self._view, model)).I.T)

        self._prog_visualize['t'] = self._timer.getElapsed()

        self._prog_visualize.draw(gl.GL_TRIANGLES, self._indices)


        gl.glDisable(gl.GL_DEPTH_TEST)


        self._planeProg['ft'] = simulation.PhiValue
        self._planeProg['ft'].interpolation = (gl.GL_NEAREST, gl.GL_LINEAR)[1]
        self._planeProg.draw(gl.GL_TRIANGLES, self._plane[1])


        SpaceWidth, SpaceHeight = self._SpaceDim
        self.write("Space {}x{}".format(SpaceWidth, SpaceHeight))
        self.write("t={}".format(round(self._timer.getElapsed(), 2)))
        self.write("dt={}".format(round(frame_dt, 2)))

        if frame_dt != 0:
            self.write("{} FPS ({}sec/frame)".format(round(1 / frame_dt), round(frame_dt, 4)))
        self._console.draw()

        if self._showHelp:
            self._helpLabels.draw()

    def resize(self, w, h):
        self._prog_visualize['projection'] = glm.perspective(45.0, w / float(h), 2.0, 100.0)

    def cycleLighting(self):
        self._currentLightMode = (self._currentLightMode+1) % self.__totalLightModes

        lHeight = 5
        if self._currentLightMode == self.__LIGHTMODE_LIGHTOFF:
            self._prog_visualize['lightOn'] = 0
        elif self._currentLightMode == self.__LIGHTMODE_LIGHTON_WHITE:
            self._prog_visualize['lightOn'] = 1
            self._prog_visualize['light1_position'] = 1, 0, 0 + lHeight
            self._prog_visualize['light2_position'] = 1, 0, 0 + lHeight
            self._prog_visualize['light3_position'] = 1, 0, 0 + lHeight
        elif self._currentLightMode == self.__LIGHTMODE_LIGHTON_WITHCOLORS:
            self._prog_visualize['lightOn'] = 1
            self._prog_visualize['light1_position'] = 1, 0, 0 + lHeight
            self._prog_visualize['light2_position'] = 0, 1, 0 + lHeight
            self._prog_visualize['light3_position'] = -1, -1, 0 + lHeight

    def increaseGridSubdivs(self):
        self._gridSubdivs *= 2
        self._prog_visualize['gridSubdivs'] = self._gridSubdivs

        return self._gridSubdivs

    def decreaseGridSubdivs(self):
        if not self._gridSubdivs == 1:
            self._gridSubdivs /= 2

        self._prog_visualize['gridSubdivs'] = self._gridSubdivs
        return self._gridSubdivs

    def swapFieldVisualization(self):
        self._viewMode = (self._viewMode+1) % 3
        self._prog_visualize['viewMode'] = self._viewMode

    def toggleHelp(self):
        self._showHelp = not self._showHelp

        return self._showHelp

    def toggleLevelLines(self):
        self._showLevelLines = not self._showLevelLines
        self._prog_visualize['showLevelLines'] = self._showLevelLines


