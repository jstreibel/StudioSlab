import numpy


class ObjectInstance:
    def __init__(self, vertices, indices, transform4x4=numpy.identity(4), texture=numpy.zeros((1,1))):
        self.vertices = vertices
        self.indices = indices
        self.transform = transform4x4
        self.texture = texture
