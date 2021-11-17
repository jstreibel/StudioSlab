from numpy import zeros, float32
from glumpy import gl, gloo
import os


class Surface(object):
    def __init__(self, width, height, depth, interpolation=gl.GL_NEAREST):
        self.texture = zeros((height, width, depth), float32).view(gloo.TextureFloat2D)

        self.texture.interpolation = interpolation
        self.framebuffer = gloo.FrameBuffer(color=self.texture)
        self.clear()

    def clear(self):
        self.activate()
        gl.glClearColor(0, 0, 0, 0)
        gl.glClear(gl.GL_COLOR_BUFFER_BIT)
        self.deactivate()

    def activate(self):
        self.framebuffer.activate()

    def deactivate(self):
        self.framebuffer.deactivate()


class Slab(object):
    def __init__(self, width, height, depth, interpolation=gl.GL_NEAREST):
        self.Input = Surface(width, height, depth, interpolation)
        self.Output = Surface(width, height, depth, interpolation)

    def swap(self):
        self.Input, self.Output = self.Output, self.Input


def FieldProgram(fragment: str, xmin=-1., xmax=1., ymin=-1., ymax=1., shaderDir="./") -> type(gloo.Program):
    # TODO botar aqui direto o codigo do bypass.vert

    if not os.path.exists(shaderDir + fragment):
        raise FileNotFoundError(shaderDir + fragment + " pwd: " + os.getcwd())

    if not os.path.exists(shaderDir + "Bypass.vert"):
        raise FileNotFoundError(shaderDir + "Bypass.vert" + " pwd: " + os.getcwd())

    program = gloo.Program(shaderDir+"Bypass.vert", shaderDir+fragment, count=4, version='460')
    program['Position'] = [(-1,-1), (-1,+1), (+1,-1), (+1,+1)]
    program['SpaceCoord'] = [(xmin,ymin), (xmin,ymax), (xmax,ymin), (xmax,ymax)]
    return program
