
from glumpy import gl
from .SurfaceSlab import Slab, FieldProgram


ShaderDir = './Simulation/gpu/'


class Simulation(object):
    def __init__(self, dt: float, GridDim=(512,512), SpaceDim=(4,4)) -> None:
        super().__init__()

        self._GridDim = GridDim
        self._SpaceDim = SpaceDim

        GridWidth, GridHeight = GridDim

        self._Phi = Slab(GridWidth, GridHeight, 4)

        self._prog_StepSim = FieldProgram(ShaderDir+'bypass.vert', ShaderDir+'StepSim.frag')
        self._prog_StepSim["GridSize"] = GridWidth, GridHeight
        self._prog_StepSim["InverseGridSize"] = 1.0 / GridWidth, 1.0 / GridHeight
        self._prog_StepSim['SpaceSize'] = SpaceDim

        self._dt = dt
        self._stepCounter = 0

    def applyInitConditions(self):
        GridWidth, GridHeight = self._GridDim
        gl.glViewport(0, 0, GridWidth, GridHeight)

        SpaceWidth, SpaceHeight = self._SpaceDim
        prog_initCond = FieldProgram(ShaderDir+'bypass.vert', ShaderDir+'initial_conditions.frag',
                                     -SpaceWidth * .5, SpaceWidth * .5, -SpaceHeight *.5, SpaceHeight* .5)
        prog_initCond['SpaceDim'] = self._SpaceDim

        self._Phi.Input.activate()
        prog_initCond.draw(gl.GL_TRIANGLE_STRIP)
        self._Phi.Input.deactivate()

    def stepSim(self, dt, nSteps=1):
        GridWidth, GridHeight = self._GridDim

        gl.glViewport(0, 0, GridWidth, GridHeight)
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glDisable(gl.GL_BLEND)

        self._stepCounter += 1
        Phi = self._Phi
        prog_Stepper = self._prog_StepSim

        for step in range(nSteps):
            self._stepCounter += 1

            Phi.Output.activate()
            prog_Stepper["thetaField"] = Phi.Input.texture
            prog_Stepper["thetaField"].interpolation = gl.GL_NEAREST
            #prog_Stepper['thetaField'].wrapping = gl.GL_REPEAT
            prog_Stepper['thetaField'].wrapping = gl.GL_CLAMP
            prog_Stepper["t"] = self.elTime
            prog_Stepper["dt"] = dt
            prog_Stepper.draw(gl.GL_TRIANGLE_STRIP)
            Phi.Output.deactivate()
            Phi.swap()

    @property
    def PhiValue(self):
        return self._Phi.Input.texture

    @property
    def dt(self):
        return self._dt

    @property
    def elTime(self) -> float:
        return self._stepCounter*self._dt

