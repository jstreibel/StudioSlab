from .Grid import Grid

from ..Tools.Primitives import Plane

import glumpy.gloo
from glumpy import gl, gloo, glm
from glumpy.transforms import TrackballPan, Position
from glumpy.app import clock, Viewport
from glumpy.geometry import primitives, parametric
import imgui

import numpy as np

from pathlib import Path


from os.path import exists


class Viewer3D(object):
    DATAMODE_FULL2D: int = 1


    class UI:

        # *************************************************************************************
        # SHADING
        _shading: (bool, bool, bool) = (True, False, False)


        # *************************************************************************************
        # LIGHT MODES
        TOTAL_LIGHTMODES = 2
        LIGHTMODE_COLORS = 0
        LIGHTMODE_WHITE = 1

        _currentLightMode = LIGHTMODE_WHITE
        _lightOn: bool = True

        def cycleLighting(self):
            self._currentLightMode = (self._currentLightMode + 1) % self.TOTAL_LIGHTMODES

            lHeight = 0
            if self._currentLightMode == self.LIGHTMODE_WHITE:
                self.program['light1_position'] = 1, 0, 0 + lHeight
                self.program['light2_position'] = 1, 0, 0 + lHeight
                self.program['light3_position'] = 1, 0, 0 + lHeight
            elif self._currentLightMode == self.LIGHTMODE_COLORS:
                self.program['light1_position'] = 1, 0, 0 + lHeight
                self.program['light2_position'] = 0, 1, 0 + lHeight
                self.program['light3_position'] = -1, -1, 0 + lHeight


        # *************************************************************************************
        # GRID TYPES

        TOTAL_GRIDTYPES: int = 3
        _gridType: int = 0
        _gridSubdivs: int = 8  #  POWER OF TWO

        def cycleGridType(self):
            self._gridType = (self._gridType + 1) % self.TOTAL_GRIDTYPES
            self.program['gridType'] = self._gridType

        def increaseGridSubdivs(self):
            self._gridSubdivs *= 2
            self.program['gridSubdivs'] = self._gridSubdivs

            return self._gridSubdivs

        def decreaseGridSubdivs(self):
            if self._gridSubdivs != 1:
                self._gridSubdivs /= 2

            self.program['gridSubdivs'] = self._gridSubdivs
            return self._gridSubdivs


        # *************************************************************************************
        # LEVEL LINES

        _showLevelLines: bool = False

        def toggleLevelLines(self):
            self._showLevelLines = not self._showLevelLines
            self.program['showLevelLines'] = self._showLevelLines


        # *************************************************************************************
        # VIEW MODE (phi OR dphidt)

        _viewMode: int = 1

        def swapFieldVisualization(self):
            self._viewMode = (self._viewMode + 1) % 3
            self.program['viewMode'] = self._viewMode


        # *************************************************************************************
        # FIELD SCALING

        _vertPhiScale: float = 1e0
        _vertDPhidtScale: float = 1e0
        _fragPhiScale: float = 1e0
        _fragDPhidtScale: float = 1e0


        # *************************************************************************************
        # INITIALIZATION AND METHODS

        def __init__(self, program: glumpy.gloo.Program):
            self.setProgram(program)

        def __init__(self):
            pass

        def setProgram(self, program: glumpy.gloo.Program):
            self.program = program

            self.cycleLighting()
            program['showLevelLines'] = self._showLevelLines

            program['gridType'] = self._gridType
            program['gridSubdivs'] = self._gridSubdivs
            self.cycleGridType()

            # self._prog_visualize['viewMode'] = self._viewMode

            program["phiScale"] = self._vertPhiScale
            program["dPhidtScale"] = self._vertDPhidtScale

            program['lightOn'] = self._lightOn

        def draw_menu(self):
            imgui.begin_group()
            imgui.text('Shading')
            shading = self._shading
            shading = imgui.radio_button('Regular', shading[0]), \
                      imgui.radio_button('Normals', shading[1]), \
                      imgui.radio_button('Texture coordinates', shading[2])
            if any(shading):
                self._shading = shading
                self.program['shading'] = np.where(shading)[0]
            imgui.end_group()

            clicked, lightOn = imgui.checkbox("Light on", self._lightOn)
            if clicked:
                self._lightOn = lightOn
                self.program['lightOn'] = lightOn

            if imgui.button("Cycle lighting"):
                self.cycleLighting()

            changed, value = imgui.slider_float('vert.phi.scale', self._vertPhiScale, 3.e-2, 10.e0)
            if changed:
                self._vertPhiScale = value
                self.program['phiScale'] = value

            changed, value = imgui.slider_float('vert.dphidt.scale', self._vertDPhidtScale, 3.e-3, 1.e0)
            if changed:
                self._vertDPhidtScale = value
                self.program['dPhidtScale'] = value


    def __init__(self, viewport: Viewport, GridDim: (int,int)=(512,512), SpaceDim: (float,float)=(1.0,1.0), MeshSubdivs: (int,int)=(512,512), surface="plane", ui: UI=UI()) -> None:
        super().__init__()

        self.viewport = viewport # is also the event dispatcher

        self.SpaceDim = SpaceDim
        self.GridDim = GridDim
        self.MeshSubdivs = MeshSubdivs


        view = np.eye(4, dtype=np.float32)
        model = np.eye(4, dtype=np.float32)
        # projection = np.eye(4, dtype=np.float32)
        glm.translate(view, 0, 0, -SpaceDim[0])
        normal = np.array(np.matrix(np.dot(view, model)).I.T)


        thisScriptsFolder = Path(__file__).parent.resolve()
        shaderDir = str(thisScriptsFolder) + "/libglsl/"
        vertexFile = shaderDir + "FieldShading.vert"
        fragmentFile = shaderDir + "FieldShading.frag"
        for file in (vertexFile, fragmentFile):
            if not exists(file):
                raise FileNotFoundError(file)


        self.fieldPlaneProgram = gloo.Program(vertexFile, fragmentFile, version='460')

        if type(surface) is str:
            if surface == "plane":
                # self.fieldPlaneData, self._indices = primitives.plane(SpaceDim, MeshSubdivs)
                self.fieldPlaneData, self._indices = Plane(SpaceDim, MeshSubdivs)
            else:
                raise Exception("Unknown type")
        elif callable(surface):
            # self.planeSectionData = ParametricSurface(lambda u, v: asarray((0, u, v)), umin=umin, umax=umax,
            #                                                                            vmin=-0.5, vmax=2,
            #                                                                            ucount=2, vcount=2)
            umin = -SpaceDim*.5
            umax = -umin
            vmin = -SpaceDim*.5
            vmax = -vmin
            self.fieldPlaneData, self._indices = parametric.surface(surface, umin=umin, umax=umax, vmin=vmin, vmax=vmax,
                                                                    ucount=MeshSubdivs, vcount=MeshSubdivs)


        self.fieldPlaneProgram.bind(self.fieldPlaneData)

        # self.fieldPlaneProgram['field'] = np.zeros((1,1))

        self.fieldPlaneProgram['model'] = model
        self.fieldPlaneProgram['view'] = view
        self.fieldPlaneProgram['normal'] = normal

        a = 0.5
        b = 1 - a
        self.fieldPlaneProgram['light1_color'] = a, b, 0
        self.fieldPlaneProgram['light2_color'] = 0, a, b
        self.fieldPlaneProgram['light3_color'] = b, 0, a

        self._view = view
        self._angs = [360, 530]

        self.fieldPlaneProgram['dr_tex'] = 1./GridDim[0], 1./GridDim[1]
        self.fieldPlaneProgram['dr'] = SpaceDim[0]/GridDim[0], SpaceDim[1]/GridDim[1]

        trackball = TrackballPan(Position('v_position'), znear=0.1, zfar=500, distance=5)
        self.fieldPlaneProgram['transform'] = trackball


        self.viewport.attach(self.fieldPlaneProgram['transform'])

        trackball.zoom = 45
        trackball.distance = 1.5 * SpaceDim[0]

        self._trackball = trackball

        ui.setProgram(self.fieldPlaneProgram)
        self._UI = ui


        grid = Grid(viewport, trackball, title=".")

        self.grid = grid


    def draw(self, texture_data=None):
        clock.tick()

        x, y, w, h = self.viewport.extents
        gl.glViewport(int(x), int(y), w, h)
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_BLEND)
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)

        # gl.glClear(gl.GL_COLOR_BUFFER_BIT)
        # gl.glClear(gl.GL_DEPTH_BUFFER_BIT)

        theta = self._angs[0]
        phi = self._angs[1]
        model = np.eye(4, dtype=np.float32)
        glm.rotate(model, theta, 0, 0, 1)
        glm.rotate(model, phi, 0, 1, 0)
        self.fieldPlaneProgram['normal'] = np.array(np.matrix(np.dot(self._view, model)).I.T)
        if texture_data is not None:
            self.fieldPlaneProgram['field'] = texture_data
            self.fieldPlaneProgram['field'].wrapping = (gl.GL_CLAMP_TO_EDGE,
                                                        gl.GL_CLAMP_TO_BORDER,
                                                        gl.GL_MIRRORED_REPEAT,
                                                        gl.GL_REPEAT,
                                                        gl.GL_MIRROR_CLAMP_TO_EDGE)[0]
            self.fieldPlaneProgram['field'].interpolation = gl.GL_LINEAR

        self.fieldPlaneProgram.draw(gl.GL_TRIANGLES, self._indices)

        # self._planeProg.draw(gl.GL_TRIANGLES, self._plane[1])

        gl.glDisable(gl.GL_DEPTH_TEST)

        self.grid.draw()


    def draw_menu(self):
        imgui.begin("Visualization", closable=False)

        tess = self.MeshSubdivs
        imgui.text("Mesh {}x{} subdivisions".format(tess[0], tess[1]))
        imgui.text("{:.2f} FPS".format(clock.get_fps()))

        imgui.dummy(.0, 20)

        self._UI.draw_menu()

        imgui.dummy(.0, 20)

        self.grid.draw_menu()

        imgui.end()
