from glumpy import app
from glumpy.app import Viewport as GlViewport

from numpy import pi, sqrt

from PySLib.Viewers2D import Viewer3D

from Simulation import Simulation


WindowWidth = int(1920*1.8)
WindowHeight = int(1080*1.8)

GridDim = 2 ** 10
SpaceDim = 4
MeshSubdivs = 2 ** 8

DiracDelta_E = .0
DiracDelta_eps = .0
#DiracDelta_E = 2500.0  # do artigo
#DiracDelta_eps = 0.1   # do artigo
#DiracDelta_E = 0.001 # ~oscillon
#DiracDelta_eps = 0.2 # ~oscillon

DiracDelta_a = sqrt((4. / 3.) * pi * (DiracDelta_eps ** 2) * DiracDelta_E)


r = 1e-3  # dt/h = r

h = float(SpaceDim / GridDim)
dt = r * h


app.use("glfw_imgui")  # Required for ImGui integration
window = app.Window(width=WindowWidth, height=WindowHeight,
                    color=(0.30, 0.30, 0.35, 1.00))



vp = GlViewport((WindowWidth, WindowHeight))
window.attach(vp)
ui = Viewer3D.UI()
ui._vertPhiScale = 0.01
ui._vertDPhidtScale = 0.001
viewer3D = Viewer3D(viewport=vp, GridDim=(GridDim, GridDim), SpaceDim=(SpaceDim, SpaceDim),
                    MeshSubdivs=(MeshSubdivs, MeshSubdivs), ui=ui)


sim = Simulation.Simulation(dt, GridDim=(GridDim, GridDim), SpaceDim=(SpaceDim, SpaceDim))
sim.applyInitConditions(DiracDelta_eps, DiracDelta_a)


imgui = window.imgui
io = imgui.get_io()
font_scaling_factor = 1
#font_size_in_pixels = 30
#font  = io.fonts.add_font_from_file_ttf(
#        "../../Resources/Fonts/Roboto-Regular.ttf", font_size_in_pixels * font_scaling_factor,
#        io.fonts.get_glyph_ranges_latin())
io.font_global_scale = font_scaling_factor
#window.imguiRenderer.refresh_font_texture()
#imgui.push_font(font)


@window.event
def on_draw(frame_dt):
    global dt, font

    sim.stepSim(dt)

    window.clear()

    viewer3D.draw(sim.PhiValue)
    #textureViewer.draw(sim.PhiValue)

    imgui.new_frame()

    if imgui.begin_main_menu_bar():
        if imgui.begin_menu("File", True):
            clicked, selected = imgui.menu_item("Quit", 'Esc', False, True)
            if clicked:
                exit(0)
            imgui.end_menu()

        imgui.end_main_menu_bar()

    viewer3D.draw_menu()
    sim.draw_menu()

    imgui.end_frame()

    imgui.render()


@window.event
def on_key_press(symbol, modifiers):
    if symbol == ord(' '):
        sim.runSim = not sim.runSim


app.run()
