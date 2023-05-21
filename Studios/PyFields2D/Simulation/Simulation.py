from glumpy import gl
from PySLib.Tools.SurfaceSlab import Slab, FieldProgram
import imgui


class Simulation(object):
    def __init__(self, dt: float, GridDim=(512, 512), SpaceDim=(4, 4)) -> None:
        super().__init__()

        self._GridDim = GridDim
        self._SpaceDim = SpaceDim

        GridWidth, GridHeight = GridDim

        self.field = Slab(GridWidth, GridHeight, 2)

        fragShader = ("StepSim.frag", "StepSim_SWInduzida.frag")[1]
        self.SimStepper = FieldProgram(fragShader, -SpaceDim[0] * .5, SpaceDim[0] * .5,
                                       -SpaceDim[0] * .5, SpaceDim[0] * .5, shaderDir='./Simulation/')

        h = SpaceDim[0] / GridWidth
        self.SimStepper['drTex'] = 1. / GridWidth
        self.SimStepper['invhsqr'] = 1. / (h * h)

        self._dt = dt
        self._stepCounter = 0
        self._elTime = 0.0
        self.nStepsPerRenderFrame: int = 150

        self.runSim = False

    def applyInitConditions(self, DiracDelta_eps=0.1, DiracDelta_a=1e1):
        GridWidth, GridHeight = self._GridDim
        gl.glViewport(0, 0, GridWidth, GridHeight)

        SpaceWidth, SpaceHeight = self._SpaceDim
        InitCond = FieldProgram("IC.frag", -SpaceWidth * .5, SpaceWidth * .5,
                                -SpaceHeight * .5, SpaceHeight * .5,
                                shaderDir='./Simulation/')

        InitCond["eps"] = DiracDelta_eps
        InitCond["a"] = DiracDelta_a

        self.field.Input.activate()
        InitCond.draw(gl.GL_TRIANGLE_STRIP)
        self.field.Input.deactivate()

    def stepSim(self, dt):
        if not self.runSim or dt*self._stepCounter==9.9:
            return

        GridWidth, GridHeight = self._GridDim

        gl.glViewport(0, 0, GridWidth, GridHeight)
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glDisable(gl.GL_BLEND)

        field = self.field
        SimStepper = self.SimStepper

        for _ in range(self.nStepsPerRenderFrame):
            self._stepCounter += 1

            field.Output.activate()

            SimStepper['dt'] = dt
            SimStepper['t'] = dt * self._stepCounter

            SimStepper['field'] = field.Input.texture
            SimStepper['field'].interpolation = gl.GL_NEAREST
            SimStepper['field'].wrapping = gl.GL_REPEAT

            SimStepper.draw(gl.GL_TRIANGLE_STRIP)

            field.Output.deactivate()
            field.swap()

        self._elTime = dt * self._stepCounter

    def _step(self, Phi, SimStepper, dt):
        pass

    @property
    def PhiValue(self):
        return self.field.Input.texture

    @property
    def dt(self):
        return self._dt

    @property
    def elTime(self) -> float:
        return self._stepCounter * self._dt

    def draw_menu(self):
        imgui.begin('Simulation')
        # changed, t = imgui.slider_float('t', self.currentSimTime, 0, self.simData['t'])
        # if changed:
        #    self.currentSimTime = t
        #    self.currentData = self.simData.getAtTime(t)

        imgui.text("GPU data format: " + str(self.PhiValue.gpu_format))

        nw, nh = self._GridDim
        imgui.text("Sim grid: {}x{}".format(nw, nh))

        sw, sh = self._SpaceDim
        imgui.text("Domain: {}x{} with origin in center".format(sw, sh))

        imgui.dummy(.0, 20)

        clicked, state = imgui.checkbox('pause', not self.runSim)
        if clicked:
            self.runSim = not state

        changed, value = imgui.slider_int("Steps/frame", self.nStepsPerRenderFrame, 1, 1000)
        if changed: self.nStepsPerRenderFrame = value

        imgui.dummy(.0, 20)

        imgui.text("t = {:.5f}".format(self._elTime))
        imgui.text("step = {}".format(self._stepCounter))

        imgui.end()
