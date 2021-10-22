import OpenGL.raw.GL.VERSION.GL_3_0
import gloo
import glumpy

from IMGUIPlayer import ImGuiPlayerApp
import imgui, glfw, glumpy.gl
from Viewers2D.Viewer3D import Viewer3D
from glumpy.app import Viewport
from glumpy.gloo import Texture2D, Program
from scipy import interpolate
import numpy as np


from DataAnalysis.OpenSim import OpenSimData, PrintDataOverview


class Field2DViewer(ImGuiPlayerApp):

    def __init__(self):
        super(Field2DViewer, self).__init__()

        fileLocation = '/home/joao/Data/workdir/2d/Analytic and numeric match/'

        simData = OpenSimData(fileLocation + 'history-N2048.osc')
        self.simData = simData
        PrintDataOverview(simData)
        self.currentSimTime = 5.0

        currData = simData.getAtTime(self.currentSimTime)
        self.currentData = currData

        DataDim = simData.getShape()
        GridDim=DataDim[1]
        SpaceDim = simData['L']
        MeshSubdivs=512

        viewerWindow = Viewport((1024, int(9.*1024/16)))
        self.vWin = viewerWindow
        self.attach(viewerWindow)
        self.viewer3D = Viewer3D(window=viewerWindow, GridDim=GridDim, SpaceDim=SpaceDim, MeshSubdivs=MeshSubdivs, mode=Viewer3D.DATAMODE_RADIAL1D)

    def window_size_callback(self, window, width, height):
        super().window_size_callback(window, width, height)

        self.viewer3D.resize(width, height)

    def on_draw(self):
        super().on_draw()

        #if self.currentData.ndim == 1:
        #    self.currentData = self._generateRadialData(self.currentData[:])

        self.viewer3D.draw(self.currentData)

    def menu(self):
        super().menu()

        imgui.new_frame()

        if imgui.begin_main_menu_bar():
            if imgui.begin_menu("File", True):
                clicked, selected = imgui.menu_item("Quit", 'Ctrl+Q', False, True)
                if clicked:
                    exit(0)  # TODO release resources D:' !!!
                imgui.end_menu()

            imgui.end_main_menu_bar()


        imgui.begin('Simulation data')
        changed, t = imgui.slider_float('t', self.currentSimTime, 0, self.simData['t'])
        if changed:
            self.currentSimTime = t
            self.currentData = self.simData.getAtTime(t)
        imgui.end()

        self.viewer3D.draw_menu()

        #imgui.show_demo_window()
