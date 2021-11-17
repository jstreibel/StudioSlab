from numpy import zeros, float32
from glumpy import gl, gloo


class Surface(object):
    def __init__(self, width, height, depth, interpolation=gl.GL_NEAREST):
        """ depth diz o numero de canais no output. Deve ser algo como 1, 2, 3, 4 -> r, r+g, r+g+b, r+g+b+a """
        self.texture = zeros((height,width,depth), float32).view(gloo.TextureFloat2D)
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


def FieldProgram(vertexShader: str, fragmentShader: str, xmin=-1, xmax=1, ymin=-1, ymax=1) -> type(gloo.Program):
    import os

    if not os.path.exists(vertexShader):
        raise FileNotFoundError(vertexShader)
    elif not os.path.exists(fragmentShader):
        raise FileNotFoundError(fragmentShader)


    program = gloo.Program(vertexShader, fragmentShader, count=4)
    program['Position'] = [(-1,-1), (-1,+1), (+1,-1), (+1,+1)]
    program['SpaceCoord'] = [(xmin,ymin), (xmin,ymax), (xmax,ymin), (xmax,ymax)]
    return program
