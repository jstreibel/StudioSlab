# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

from vispy import app, gloo
app.use_app('pyqt5')
from vispy.gloo import Program

folder = "/home/joao/Developer/OpenGLStudies/shade/"

with open(folder) as file:
    vertex = file.read()

with open("/home/joao/Developer/Shader_fooling/plots.frag") as file:
    fragment = file.read()

class Canvas(app.Canvas):
    def __init__(self):
        super().__init__(size=(512, 512), title='Colored quad',
                         keys='interactive')

        # Build program
        self.program = Program(vertex, fragment, count=4)

        # Set uniforms and attributes
        self.program['color'] = [(1, 0, 0, 1), (0, 1, 0, 1),
                                 (0, 0, 1, 1), (1, 1, 0, 1)]
        self.program['position'] = [(-1, -1), (-0.5, +0.5),
                                    (+1, -1), (+1, +1)]
        #self.program['samples'] = [0 ,0 ,0, 1 ,0 ,0, 0 ,1 ,0, 0 ,0 ,1, 1 ,1 ,1]

        gloo.set_viewport(0, 0, *self.physical_size)

        self.show()

    def on_draw(self, event):
        gloo.clear()
        self.program.draw('triangle_strip')

    def on_resize(self, event):
        gloo.set_viewport(0, 0, *event.physical_size)

c = Canvas()
app.run()

