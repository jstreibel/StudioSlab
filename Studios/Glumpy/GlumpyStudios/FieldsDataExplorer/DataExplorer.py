#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse

import numpy as np
from glumpy import app
from glumpy.app import Viewport as GlViewport
from glumpy.gloo import TextureFloat2D


from Viewers2D import Viewer3D


WindowWidth = int(1920)
WindowHeight = int(1080)


app.use("glfw_imgui")  # Required for ImGui integration
window = app.Window(width=WindowWidth, height=WindowHeight,
                    color=(0.30, 0.30, 0.35, 1.00))


imgui = window.imgui
io = imgui.get_io()
font_scaling_factor = 2
io.font_global_scale = font_scaling_factor


viewer3D = None
simDataTexture = None


@window.event
def on_draw(frame_dt):
    global dt, font

    window.clear()

    viewer3D.draw(simDataTexture)

    imgui.new_frame()

    if imgui.begin_main_menu_bar():
        if imgui.begin_menu("File", True):
            clicked, selected = imgui.menu_item("Quit", 'Esc', False, True)
            if clicked:
                exit(0)
            imgui.end_menu()

        imgui.end_main_menu_bar()

    viewer3D.draw_menu()

    imgui.end_frame()

    imgui.render()


@window.event
def on_key_press(symbol, modifiers):
    pass


def run(fieldsOscbFile: str):
    from SimData.open_sim import OpenSimData
    from SimData import SimData
    global viewer3D, simDataTexture

    simData: SimData = OpenSimData(fieldsOscbFile)
    metaData = simData.MetaData
    simDataTexture = np.ascontiguousarray(simData.Data).view(TextureFloat2D)

    resX, resT = metaData['outresX'], metaData['outresT']
    L, t = metaData['L'], metaData['t']
    GridDim = resX, resT
    SpaceDim = L, t
    MeshSubdivs = resX, resT

    vp = GlViewport((WindowWidth, WindowHeight))
    window.attach(vp)
    ui = Viewer3D.UI()
    ui._vertPhiScale = 1.0
    ui._vertDPhidtScale = 0.10
    viewer3D = Viewer3D(viewport=vp, GridDim=GridDim, SpaceDim=SpaceDim, MeshSubdivs=MeshSubdivs, ui=ui)

    app.run()


if __name__ == '__main__':
    import sys

    if len(sys.argv) != 2:
        print("\nUsage: data-explorer [filename]\n")
        exit(2)

    run(sys.argv[1])