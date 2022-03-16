# -----------------------------------------------------------------------------
# Copyright (c) 2009-2016 Nicolas P. Rougier. All rights reserved.
# Distributed under the (new) BSD License.
# -----------------------------------------------------------------------------
# High frequency (below pixel resolution) function plot
#
#  -> http://blog.hvidtfeldts.net/index.php/2011/07/plotting-high-frequency-functions-using-a-gpu/
#  -> https://www.shadertoy.com/view/4sB3zz
# -----------------------------------------------------------------------------
import imgui
import numpy
import numpy as np
from numpy import pi, sin
from glumpy import app, gl, gloo
import pygame

vertex = """
attribute vec2 position;
void main (void)
{
    gl_Position = vec4(position, 0.0, 1.0);
}
"""

fragment = """
uniform vec2 iResolution;

uniform sampler1D A;
uniform sampler1D k;
uniform int fourierSamples;

const float pi = 3.1416;


float function( float x )
{       
    float val = .0;
    
    float du = 1./float(fourierSamples);
    
    for(int i=0; i<fourierSamples; ++i){
        float u = i*du + .5*du;
        
        float _A = texture1D(A, u).r;
        float _k = texture1D(k, u).r;
        
        val += _A*sin(2*pi*_k*x);
    }
        
    return val;

}


float sample(vec2 uv)
{
    const int samples = 16;
    const float fsamples = float(samples);
    vec2 maxdist = vec2(0.5,1.0)/50.0;
    vec2 halfmaxdist = vec2(0.5) * maxdist;

    float stepsize = maxdist.x / fsamples;
    float initial_offset_x = -0.5 * fsamples * stepsize;
    uv.x += initial_offset_x;
    float hit = 0.0;
    for( int i=0; i<samples; ++i )
    {
        float x = uv.x + stepsize * float(i);
        float y = uv.y;
        float fx = function(x);
        float dist = abs(y-fx);
        hit += step(dist, halfmaxdist.y);
    }
    
    const float arbitraryFactor = 4.5;
    const float arbitraryExp = 0.95;
    return arbitraryFactor * pow( hit / fsamples, arbitraryExp );
}

void main(void)
{
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    
    float ymin = -2.0;
    float ymax = +2.0;
    float xmin = 0.0;
    float xmax = xmin + pi;

    vec2 xy = vec2(xmin,ymin) + uv*vec2(xmax-xmin, ymax-ymin);
    gl_FragColor = vec4(0,0,0, sample(xy));
}
"""


app.use("glfw_imgui")
window = app.Window(width=2000, height=1000, color=(1,1,1,1))
pause = False


sampleRate = 44100
freq = 440

pygame.mixer.init(sampleRate, -16, 2, 512)

A = lambda k: 1./k**2

fourierMap = np.asarray([(A(k), float(k)) for k in range(1, 35)])


def func(t):
    global fourierMap

    val = .0
    for pair in fourierMap:
        A = pair[0]
        k = pair[1]

        val += A * sin(pi * k * freq * t)

    return val

arr = numpy.array([4096 * func(t/sampleRate) for t in range(0, sampleRate)]).astype(numpy.int16)

arr = numpy.c_[arr,arr]

sound = pygame.sndarray.make_sound(arr)


@window.event
def on_draw(dt):
    window.clear()
    program.draw(gl.GL_TRIANGLE_STRIP)

    imgui.new_frame()

    if imgui.begin_main_menu_bar():
        if imgui.begin_menu("File", True):
            clicked, selected = imgui.menu_item("Quit", 'Esc', False, True)
            if clicked:
                exit(0)
            imgui.end_menu()

        imgui.end_main_menu_bar()

    imgui.begin("Fourier", closable=False)
    imgui.text("Sample rate {}".format(sampleRate))
    imgui.text("Base freq {}".format(freq))
    imgui.text("Fourier samples {}".format(len(fourierMap)))
    imgui.end()

    imgui.end_frame()
    imgui.render()


@window.event
def on_key_press(key, modifiers):
    global pause
    if key == ord(' '):
        pause = not pause

@window.event
def on_resize(width, height):
    program["iResolution"] = width, height

program = gloo.Program(vertex, fragment, count=4)
xmin = -1
xmax = 1
ymin = -1
ymax = 1


print('A_k:', fourierMap[:,0])
print('k:', fourierMap[:,1])


program['A'] = fourierMap[:,0]
program['A'].interpolation = gl.GL_NEAREST
program['k'] = fourierMap[:,1]
program['k'].interpolation = gl.GL_NEAREST
program['fourierSamples'] = len(fourierMap)
program['position'] = [(xmin,ymin), (xmin,ymax), (xmax,ymin), (xmax,ymax)]

sound.play(-1)

app.run()