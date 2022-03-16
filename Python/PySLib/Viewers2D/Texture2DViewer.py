
from glumpy import gl, gloo
from glumpy.geometry import primitives
import os

from ..Globals import PySLibDir


class Texture2DViewer(object):


    def __init__(self, WindowDim: (float, float)) -> None:
        super().__init__()

        self._WindowDim = WindowDim

        self._initProgram()


    def _initProgram(self) -> None:
        ratio = 1080/1920

        self.renderingPlane = primitives.plane(size=2)

        #self.arrowProg = gloo.Program(ShaderDir + "PlaneShading.vert", ShaderDir + "PlaneShadingArrow.frag")
        #self.arrowProg.bind(self.renderingPlane[0])

        shaderDir = PySLibDir + "/Viewers2D/libglsl/"
        if not os.path.exists(shaderDir + "TextureView.vert"):
            raise FileNotFoundError(shaderDir + "TextureView.vert")
        if not os.path.exists(shaderDir + "TextureView.frag"):
            raise FileNotFoundError(shaderDir + "TextureView.frag")

        self.colourProg = gloo.Program(shaderDir + "TextureView.vert", shaderDir + "TextureView.frag", version='460')
        self.colourProg.bind(self.renderingPlane[0])


    interpolation=gl.GL_NEAREST
    def draw(self, texture: gloo.Texture2D):
        w, h = self._WindowDim

        gl.glViewport(0, 0, w, h)
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_BLEND)
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)

        wrapping = (gl.GL_REPEAT, gl.GL_CLAMP)[0]

        # Cores
        self.colourProg['field'] = texture
        self.colourProg['field'].wrapping = wrapping
        self.colourProg['field'].interpolation = self.interpolation
        self.colourProg.draw(gl.GL_TRIANGLES, self.renderingPlane[1])

        # Setas
        #self.arrowProg['field'] = texture
        #self.arrowProg['field'].wrapping = wrapping
        #self.arrowProg['field'].interpolation = (gl.GL_LINEAR, gl.GL_NEAREST)[0]
        #self.arrowProg.draw(gl.GL_TRIANGLES, self.renderingPlane[1])


    def resize(self, w, h):
        #self._WindowDim = w, h
        #self.arrowProg['iResolution'] = self._WindowDim
        pass
