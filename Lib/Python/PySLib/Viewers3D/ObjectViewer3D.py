import glumpy.gloo
import numpy
import numpy as np
from glumpy import gl, gloo, glm
from glumpy.transforms import TrackballPan
from glumpy.app import clock, Viewport
from glumpy.transforms import Position
from glumpy.geometry import primitives

from numpy import eye, float32, array, matrix, dot

import imgui

from Studios.Triagem import PySLibDir

from PySLib.Viewers3D import ObjectInstance


class ObjectViewer3D(object):
    DATAMODE_FULL2D: int = 1
    DATAMODE_RADIAL1D: int = 2


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
            self.program = program

            self.cycleLighting()
            program['showLevelLines'] = self._showLevelLines

            self.program['gridType'] = self._gridType
            self.program['gridSubdivs'] = self._gridSubdivs
            self.cycleGridType()

            # self._prog_visualize['viewMode'] = self._viewMode

            self.program["phiScale"] = self._vertPhiScale
            self.program["dPhidtScale"] = self._vertDPhidtScale

            self.program['lightOn'] = self._lightOn


        def draw_menu(self):
            imgui.begin_group()
            imgui.text('Shading')
            shading = self._shading
            shading = imgui.radio_button('Regular', shading[0]), \
                      imgui.radio_button('Normals', shading[1]), \
                      imgui.radio_button('Texture coordinates', shading[2])
            if any(shading):
                self._shading = shading
                self.program['shading'] = numpy.where(shading)[0]
            imgui.end_group()

            clicked, lightOn = imgui.checkbox("Light on", self._lightOn)
            if clicked:
                self._lightOn = lightOn
                self.program['lightOn'] = lightOn

            if imgui.button("Cycle lighting"):
                self.cycleLighting()

            if imgui.button("Cycle grid"):
                self.cycleGridType()

            changed, value = imgui.slider_float('vert.phi.scale', self._vertPhiScale, 1.e-2, 30.e0)
            if changed:
                self._vertPhiScale = value
                self.program['phiScale'] = value

            changed, value = imgui.slider_float('vert.dphidt.scale', self._vertDPhidtScale, 1.e-3, 3.e0)
            if changed:
                self._vertDPhidtScale = value
                self.program['dPhidtScale'] = value


    def __init__(self, viewport: Viewport) -> None:
        super().__init__()

        self.viewport = viewport # is also the event dispatcher

        view = eye(4, dtype=float32)
        model = eye(4, dtype=float32)
        glm.translate(view, 0, 0, -1.5)
        normal = array(matrix(dot(view, model)).I.T)


        shaderDir = PySLibDir + "/Viewers3D/libglsl/"

        vertexFile = shaderDir + "FieldShading.vert"
        fragmentFile = shaderDir + "FieldShading.frag"


        from os.path import exists
        for file in (vertexFile, fragmentFile):
            if not exists(file): raise FileNotFoundError(file)

        self.objectsProgram = gloo.Program(vertexFile, fragmentFile, version='460')

        self.objectsProgram['field'] = np.zeros((1, 1))

        self.objectsProgram['model'] = model
        self.objectsProgram['view'] = view
        self.objectsProgram['normal'] = normal

        self.objectsProgram['light1_position'] = 3, 0, -3
        self.objectsProgram['light2_position'] = 0, -3, 0
        self.objectsProgram['light3_position'] = 0, 0, -3
        a = 0.5
        b = 1 - a
        self.objectsProgram['light1_color'] = a, b, 0
        self.objectsProgram['light2_color'] = 0, a, b
        self.objectsProgram['light3_color'] = b, 0, a

        self._view = view
        self._angs = [360, 530]

        trackball = TrackballPan(Position('v_position'), znear=0.1, zfar=500, distance=5)
        self.objectsProgram['transform'] = trackball


        self.viewport.attach(self.objectsProgram['transform'])

        trackball.zoom = 45
        trackball.distance = 5

        self._trackball = trackball

        self._UI = ObjectViewer3D.UI(self.objectsProgram)

        self.objects = []

        planeVerts, planeIds = primitives.plane(100, uv_repeat=50)

        plane = ObjectInstance.ObjectInstance(planeVerts, planeIds, numpy.identity(4))
        self.objects.append(plane)


    def addObject3D(self, object3D: ObjectInstance.ObjectInstance):
        self.objects.append(object3D)

    def draw(self):
        # TODO: assert `data` is either 1D or 2D depending on value of self._mode

        clock.tick()

        x, y, w, h = self.viewport.extents
        gl.glViewport(int(x), int(y), w, h)
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_BLEND)
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)

        #gl.glClear(gl.GL_COLOR_BUFFER_BIT)
        #gl.glClear(gl.GL_DEPTH_BUFFER_BIT)

        theta = self._angs[0]
        phi = self._angs[1]
        model = eye(4, dtype=float32)
        glm.rotate(model, theta, 0, 0, 1)
        glm.rotate(model, phi, 0, 1, 0)
        self.objectsProgram['normal'] = array(matrix(dot(self._view, model)).I.T)

        prog = self.objectsProgram
        for obj in self.objects:
            prog.bind(obj.vertices)
            prog['model'] = obj.transform
            prog['field'] = obj.texture
            prog.draw(gl.GL_TRIANGLES, obj.indices)

        gl.glDisable(gl.GL_DEPTH_TEST)


    def draw_menu(self):
        imgui.begin("Visualization", closable=False)

        #imgui.text("Mesh {}x{} subdivisions".format(tess, tess))
        imgui.text("{:.2f} FPS".format(clock.get_fps()))

        self._UI.draw_menu()

        imgui.end()