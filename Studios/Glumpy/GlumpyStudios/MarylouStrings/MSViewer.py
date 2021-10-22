import OpenGL.raw.GL.VERSION.GL_3_0
import glumpy

import SurfaceSlab
from IMGUIPlayer import ImGuiPlayerApp
import imgui, glumpy.gl as gl
from Viewers2D.Viewer3D import Viewer3D
from glumpy.app import Viewport
from glumpy.gloo import Texture2D, Program
import SurfaceSlab


from DataAnalysis.OpenSim import OpenSimData, PrintDataOverview


class StringsEqsViewer(ImGuiPlayerApp):
    x: float = 1.0


    def __init__(self, GridDim: float, SpaceDim, phiScale: float=1, dPhidtScale: float=1):
        super(StringsEqsViewer, self).__init__()

        self.GridDim = GridDim
        self.SpaceDim = SpaceDim

        viewerWindow = Viewport((1024, int(9.*1024/16)))
        self.vWin = viewerWindow
        self.attach(viewerWindow)
        self.viewer3D = Viewer3D(viewerWindow, GridDim, SpaceDim, int(GridDim), mode=Viewer3D.DATAMODE_FULL2D)

        self.VSurface = SurfaceSlab.Surface(GridDim, GridDim, 4)  # 2 canais pra Phi (double) e dois pra dPhidt (double)

        self.VProg = SurfaceSlab.FieldProgram("ddVdx2.frag", xmin=-.5*SpaceDim, xmax=.5*SpaceDim, ymin=.5*SpaceDim, ymax=-.5*SpaceDim, shaderDir="./")

        self.redrawV()


    def redrawV(self):
        GridDim = self.GridDim

        gl.glViewport(0, 0, GridDim, GridDim)
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glDisable(gl.GL_BLEND)

        VSurface = self.VSurface
        VProg = self.VProg

        VSurface.activate()

        VProg['x'] = self.x
        VProg.draw(gl.GL_TRIANGLE_STRIP)

        VSurface.deactivate()

        self.currentData = VSurface.texture


    def window_size_callback(self, window, width, height):
        super().window_size_callback(window, width, height)

        self.viewer3D.resize(width, height)


    def on_draw(self):
        super().on_draw()

        self.viewer3D.draw(self.currentData)


    def menu(self):
        super().menu()

        imgui.new_frame()

        #if imgui.begin_main_menu_bar():
        #    if imgui.begin_menu("File", True):
        #        clicked, selected = imgui.menu_item("Quit", 'Ctrl+Q', False, True)
        #        if clicked:
        #            exit(0)  # TODO release resources D:' !!!
        #        imgui.end_menu()
#
        #    imgui.end_main_menu_bar()
#
        imgui.begin('Simulation data')
        changed, x = imgui.slider_float('x', self.x, 0, 15)
        if changed:
            self.x = x
            self.redrawV()
        imgui.end()
#
        self.viewer3D.draw_menu()

        #imgui.show_demo_window()
