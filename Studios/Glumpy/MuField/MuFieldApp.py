import numpy as np

from glumpy import gloo, gl
import glfw
import imgui


from IMGUIPlayerApp import ImGuiPlayerApp


from Simulation.Simulation import Simulation
from ViewControl.Visualization import Visualization, Console



# *******************************************************************************
# *******************************************************************************

SAMPLES                = 0

ViewportWidth          = 3840
ViewportHeight         = 2160

GridWidth              = 2**8
SpaceWidth             = 1

r = 1e-1  # dt/h = r

GridHeight             = GridWidth
SpaceHeight            = SpaceWidth


h = SpaceWidth/GridWidth
dt = r*h

# *******************************************************************************
# *******************************************************************************



class MuFieldApp(ImGuiPlayerApp):
    # ---------------------------------------------------------------------------
    # ------------------------- internal classes  -------------------------------
    # ---------------------------------------------------------------------------

    class SimUI():
        """
        This class contains the variables that are communicated
        to the vertex and fragment programs
        """

        mouse_pos: np.array = None  # mouse position; used for translating the view
        #zoom: float = None  # zoom value
        running: bool = False  # toggle animation
        #speed: float = None  # speed of animation
        theta0: float = 0  # rotation angle

        interpolations: (int, int) = (gl.GL_LINEAR, gl.GL_NEAREST)
        interpState: (bool, bool) = (False, True)

        COLOURING_ENERGY, COLOURING_THETA = 0, 1
        colourings: (int, int) = COLOURING_ENERGY, COLOURING_THETA
        colouringState: (bool, bool) = True, False

        showLevelLines: bool = False

        prev_mouse: np.array = None  # previous mouse position to implement drag; maybe move to ImGuiPlayer

        def __init__(self):
            self.mouse_pos = np.array([0, 0]).astype('float')
            self.prev_mouse = np.array([-1, -1]).astype('float')

    # ---------------------------------------------------------------------------
    # --------------------------- class variables -------------------------------
    # ---------------------------------------------------------------------------

    sim_ui: SimUI = None
    quad: gloo.Program = None
    sim: Simulation = None
    viewer: Visualization = None

    def __init__(self):
        super().__init__()
        self.sim_ui = MuFieldApp.SimUI()

        self.sim = Simulation(dt, GridDim=(GridWidth,GridHeight), SpaceDim=(SpaceWidth,SpaceHeight))
        self.viewer = Visualization(WindowDim=(ViewportWidth, ViewportHeight), GridDim=(GridWidth, GridHeight), SpaceDim=(SpaceWidth,SpaceHeight))

        self.sim.applyInitConditions()


    # ---------------------------------------------------------------------------
    # --------------------------- class methods - gui and menu   ----------------
    # ---------------------------------------------------------------------------

    def key_callback(self, window, key, scancode, action, mods):

        if key == glfw.KEY_SPACE and action == glfw.RELEASE:
            self.sim_ui.running = not self.sim_ui.running

        pass

    def mouse_cursor_pos_callback(self, window, xpos, ypos):
        io = imgui.get_io()

        mouse_pos = np.array([xpos, ypos]).astype('float')

        if io.mouse_down[0]:

            _, _, w, h = gl.glGetIntegerv(gl.GL_VIEWPORT)

            if not np.all(self.sim_ui.prev_mouse == np.array([-1, -1])):
                dmouse = self.sim_ui.prev_mouse - mouse_pos;

                #self.ui.mouse_pos -= np.array([dmouse[0] / w, -dmouse[1] / h]) / self.ui.zoom * 2
                #self.quad["mouse_pos"] = self.ui.mouse_pos

                print("mouse_pos: {}, dmouse: {}".format(mouse_pos, dmouse))

        self.sim_ui.prev_mouse = mouse_pos

    def mouse_button_callback(self, window, button, action, mods):
        pass

    def mouse_scroll_callback(self, window, xoffset, yoffset):

        #self.ui.zoom += yoffset * 0.05

        #if self.ui.zoom < 0.1:
        #    self.ui.zoom = 0.1

        #self.quad["zoom"] = self.ui.zoom

        pass

    def on_draw(self):
        '''
        called for every rendered frame
        '''

        if self.sim_ui.running:
            self.sim.stepSim(dt)

        self.viewer.draw(self.sim)

    def menu(self):

        imgui.new_frame()

        if imgui.begin_main_menu_bar():
            if imgui.begin_menu("File", True):
                clicked, selected = imgui.menu_item(
                    "Quit", 'Ctrl+Q', False, True
                )
                if clicked:
                    #glfw.set_window_should_close(self.window, True)
                    exit(0)
                imgui.end_menu()

            if imgui.begin_menu("Mode", True):
                mi = imgui.menu_item
                sim_menu_result, ic_menu_result = mi("Simulation", '', True, True), mi("Initial conditions", '', True, True)

                imgui.end_menu()

            imgui.end_main_menu_bar()


        imgui.begin("General", closable=False)
        imgui.text('Grid {}x{}'.format(GridWidth, GridHeight))
        imgui.text('Space {}x{}'.format(SpaceWidth, SpaceHeight))
        imgui.end()


        imgui.begin("Simulation", closable=False)
        changed, self.sim_ui.running = imgui.checkbox("Run sim", self.sim_ui.running)
        imgui.text('h = {}'.format(h))
        global r
        changed, value = imgui.slider_float('r', r, 1e-2, 1)
        if changed:
            global dt
            r = value
            dt = h*r
        imgui.text('dt = h*r = {}'.format(dt))

        imgui.button('reset')

        imgui.end()


        imgui.begin("Visualization", closable=False)

        #imgui.text_colored("* hold Left Mouse Button and drag to translate", 0.2, 1., 0.)
        #imgui.text_colored("* use scroll wheel to zoom", 0.4, 1., 0.)
        #imgui.text_colored("* press SPACE to toggle animation", 0.6, 1., 0.)
        imgui.slider_angle('gauge', self.sim_ui.theta0, 0, 720)


        imgui.separator()


        imgui.text("Interpolation")
        imgui.begin_group()
        rb = imgui.radio_button
        ist = self.sim_ui.interpState
        result = rb('Linear', ist[0]), rb('Nearest', ist[1])
        if any(result):
            self.sim_ui.interpState = result
            self.viewer.interpolation = self.sim_ui.interpolations[np.where(result)[0][0]]
        imgui.end_group()

        imgui.text('View mode')
        result = imgui.checkbox('Level lines', self.sim_ui.showLevelLines)
        if result[0]:
            self.sim_ui.showLevelLines = result[1]
            self.viewer.colourProg['showLevelLines'] = int(self.sim_ui.showLevelLines)

        imgui.text('Colouring')
        imgui.begin_group()
        clr = self.sim_ui.colouringState
        result = rb('Energy', clr[0]), rb('Field', clr[1])
        if any(result):
            self.sim_ui.colouringState = result
            self.viewer.colourProg['colouring'] = self.sim_ui.colourings[np.where(result)[0][0]]
        imgui.end_group()



        #changed, self.ui.gamma = imgui.slider_float("Gamma", self.ui.gamma, 0.3, 3.3)
        #imgui.text("Animation Angle: {:0.2f}".format(self.ui.theta / (2.0 * np.pi) * 360.0))
        #changed, self.ui.speed = imgui.slider_float("Speed", self.ui.speed, 0.0, np.pi / 2.0)

        # transfer changed variables to shader programs
        #self.quad['blue'] = self.ui.blue
        #self.quad['gamma'] = self.ui.gamma

        imgui.end()

    #def _generateSimUI:
