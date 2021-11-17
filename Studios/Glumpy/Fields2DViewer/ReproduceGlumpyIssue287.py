# -----------------------------------------------------------------------------
# Copyright (c) 2009-2016 Nicolas P. Rougier. All rights reserved.
# Distributed under the (new) BSD License.
# -----------------------------------------------------------------------------
import numpy as np
from glumpy import gloo

vertex = """
    attribute vec2 position;
    attribute vec2 texcoord;
    varying vec2 v_texcoord;
    void main()
    {
        gl_Position = vec4(position, 0.0, 1.0);
        v_texcoord = texcoord;
    }
"""

fragment = """
    uniform sampler1D texture;
    varying vec2 v_texcoord;
    void main()
    {
        float r = texture1D(texture, v_texcoord.x).r;
        gl_FragColor = vec4(r,r,r,1);
    }
"""

myProgram = gloo.Program(vertex, fragment, count=4)

myArray = np.linspace(0.0,1.0,256)
myProgram['texture'] = myArray
myProgram['texture'] = myArray
