from Simulation import Simulation
from . import Console

from glumpy import gl, gloo
from glumpy.geometry import surface as ParametricSurface
from glumpy.graphics.text import FontManager
from glumpy.graphics.collections import GlyphCollection

from numpy import asarray


class Visualization(object):


    def __init__(self, WindowDim: (float, float), GridDim: (float,float), SpaceDim: (float, float)) -> None:
        super().__init__()

        self._WindowDim = WindowDim
        self._SpaceDim = SpaceDim
        self._GridDim = GridDim

        self._initProgram(GridDim, SpaceDim)


    def _initProgram(self, GridDim: (float,float), SpaceDim: (float, float)) -> None:
        GridWidth, GridHeight = GridDim

        vL = 2
        umin, umax, vmin, vmax = -vL * .5, vL * .5, -vL * .5, vL * .5

        ratio = 1080/1920
        ShaderDir = "./ViewControl/gpu/"

        self.renderingPlane = ParametricSurface(lambda u, v: asarray((u, v, 0)), umin=umin, umax=umax, vmin=vmin, vmax=vmax, ucount=2, vcount=2)
        self.arrowProg = gloo.Program(ShaderDir + "PlaneShading.vert", ShaderDir + "PlaneShadingArrow.frag")
        self.arrowProg.bind(self.renderingPlane[0])
        self.arrowProg['Scale'] = (ratio, 1)

        self.colourProg = gloo.Program(ShaderDir + "PlaneShading.vert", ShaderDir + "PlaneShading.frag")
        self.colourProg.bind(self.renderingPlane[0])
        self.colourProg['tex_dr'] = 1 / GridWidth, 1 / GridHeight
        self.colourProg['SpaceSize'] = SpaceDim
        self.colourProg['Scale'] = (ratio, 1)


    interpolation=gl.GL_NEAREST
    def draw(self, simulation: Simulation.Simulation):
        w, h = self._WindowDim
        gl.glViewport(0, 0, w, h)
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_BLEND)
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)

        #window.clear()

        wrapping = (gl.GL_REPEAT, gl.GL_CLAMP)[0]

        # Cores
        self.colourProg['field'] = simulation.PhiValue
        self.colourProg['field'].wrapping = wrapping
        self.colourProg['field'].interpolation = self.interpolation
        self.colourProg.draw(gl.GL_TRIANGLES, self.renderingPlane[1])

        # Setas
        self.arrowProg['field'] = simulation.PhiValue
        self.arrowProg['field'].wrapping = wrapping
        #self.arrowProg['field'].interpolation = gl.GL_NEAREST
        self.arrowProg['field'].interpolation = gl.GL_LINEAR
        self.arrowProg.draw(gl.GL_TRIANGLES, self.renderingPlane[1])


        #SpaceWidth, SpaceHeight = self._SpaceDim
        #if frame_dt != 0:
        #    self.write("{} FPS ({}sec/frame)".format(round(1 / frame_dt), round(frame_dt, 4)))


    def resize(self, w, h):
        self._WindowDim = w, h
        self.arrowProg['iResolution'] = self._WindowDim


    def on_key_press(self, symbol, modifiers):
        if symbol == 65470:  # F1
            self.showHelp = not self.showHelp
        elif symbol == 61:  # +
            pass
        elif symbol == 45:  # -
            pass
        elif symbol == 49:  # 1
            pass
        elif symbol == 50:  # 2
            pass
        elif symbol == 70:  # f
            pass
        elif symbol == 71:  # g
            pass
        elif symbol == 76:  # l
            pass
        elif symbol == 86:  # v
            pass
