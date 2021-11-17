
from glumpy import gl
from ViewTools.SurfaceSlab import Surface, FieldProgram


class Simulation(object):
    def __init__(self, dt: float, GridDim=(512,512), SpaceDim=(4,4), FourierMapScale=1.0) -> None:
        super().__init__()

        self._GridDim = GridDim
        self._SpaceDim = SpaceDim

        GridWidth, GridHeight = GridDim

        self._Phi = Surface(GridWidth, GridHeight, 2)

        self._prog_StepSim = FieldProgram("StepSim.frag")
        self._prog_StepSim['spaceDim'] = SpaceDim
        self._prog_StepSim['S'] = FourierMapScale

        self.k0 = 1
        self.th0 = 0
        self.eps = 0.2
        self._prog_StepSim['k0'] = self.k0
        self._prog_StepSim['th0'] = self.th0
        self._prog_StepSim['eps'] = self.eps

        self._dt = dt
        self._stepCounter = 0

        self.stepSim(dt, 1)


    def stepSim(self, dt, nSteps=1):
        GridWidth, GridHeight = self._GridDim

        gl.glViewport(0, 0, GridWidth, GridHeight)
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glDisable(gl.GL_BLEND)

        Phi = self._Phi
        prog_Stepper = self._prog_StepSim
        prog_Stepper['k_'] = 4.

        for step in range(nSteps):
            self._stepCounter += 1

            Phi.activate()
            prog_Stepper["t"] = self.elTime
            prog_Stepper.draw(gl.GL_TRIANGLE_STRIP)
            Phi.deactivate()


    def deltak0(self, delta):
        k0_min = 0.1
        self.k0 += delta
        if self.k0 < k0_min:
            self.k0 = k0_min
        self._prog_StepSim['k0'] = self.k0

    def deltaTh0(self, delta):
        self.th0 += delta
        self._prog_StepSim['th0'] = self.th0

    def deltaEps(self, delta):
        self.eps += delta
        if self.eps < abs(delta):
            self.eps = abs(delta)
        self._prog_StepSim['eps'] = self.eps

    @property
    def PhiValue(self):
        return self._Phi.texture

    @property
    def dt(self):
        return self._dt

    @property
    def elTime(self) -> float:
        return self._stepCounter*self._dt

