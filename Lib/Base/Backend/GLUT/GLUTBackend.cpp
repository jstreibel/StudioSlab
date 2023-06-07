#include <GL/glew.h>
#include <GL/freeglut.h>

#include "GLUTBackend.h"

#include <Common/Resources.h>

#include "3rdParty/imgui/imgui.h"
#include "3rdParty/imgui/backends/imgui_impl_glut.h"
#include "3rdParty/imgui/backends/imgui_impl_opengl3.h"
#include "3rdParty/ImGuiColorsSetup.h"

#include "Phys/Numerics/Program/Integrator.h"
#include "Base/Graphics/Styles/StylesAndColorSchemes.h"
#include "Base/Graphics/OpenGL/Utils.h"

#include <cassert>
#include <filesystem>

GLUTBackend *GLUTBackend::glutBackend = nullptr;

//#define FORCE_FPS 60
//const Real FRAME_TIME = 1.0/Real(FORCE_FPS);

GLUTBackend::GLUTBackend() : Backend(this, "GLUT backend") {
    assert(GLUTBackend::glutBackend == nullptr);

    int dummy = 0;
    glutInit(&dummy, nullptr);

    w = 3200, h = 1350;

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    //glutInitWindowSize(w, h);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(w, h);
    //glutFullScreen();

    int winHandle = glutCreateWindow("Pendulum");

    // GLEW init:
    {
            GLenum initStatus = glewInit();
        if (GLEW_OK != initStatus) {
            char buffer[1024];
            sprintf(buffer, "Error: %s\n", glewGetErrorString(initStatus));
            throw String(buffer);
        } else {
            char buffer[1024];
            sprintf(buffer, "Using GLEW %s\n", glewGetString(GLEW_VERSION));
            std::cout << buffer << std::endl;
        }
    }

    glutDisplayFunc(GLUTBackend::render);
    glutReshapeFunc(GLUTBackend::reshape);

    glutKeyboardFunc(GLUTBackend::keyboard);
    glutSpecialFunc(GLUTBackend::keyboardSpecial);

    glutMouseFunc(GLUTBackend::mouseButton);
    glutMouseWheelFunc(GLUTBackend::mouseWheel);
    glutPassiveMotionFunc(GLUTBackend::mousePassiveMotion);
    glutMotionFunc(GLUTBackend::mouseMotion);


    glutIdleFunc(GLUTBackend::idleCall);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);


    std::cout << "Initialized GLUTBackend. Current window: " << winHandle << std::endl;


    // Setup Dear ImGui context
    // Themes: https://github.com/ocornut/imgui/issues/707

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    SetupImGuiColors_BlackAndWhite();

    // Setup Platform/Renderer backends
    // FIXME: Consider reworking this example to install our own GLUT funcs + forward calls ImGui_ImplGLUT_XXX ones, instead of using ImGui_ImplGLUT_InstallFuncs().
    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init();

    // Install GLUT handlers (glutReshapeFunc(), glutMotionFunc(), glutPassiveMotionFunc(), glutMouseFunc(), glutKeyboardFunc() etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    if(0) ImGui_ImplGLUT_InstallFuncs();


    if(1) {
        auto fontName = Resources::fontFileName(5);

        if (!std::filesystem::exists(fontName)) throw String("Font ") + fontName + " does not exist.";

        auto font = io.Fonts->AddFontFromFileTTF(fontName.c_str(), 20.0f);
        io.FontDefault = font;

        //ImGui::PushFont(font);
    }

    ImGui::GetStyle().ScaleAllSizes(1.5);
    ImGui::GetIO().FontGlobalScale = 1;

    Styles::Init();

    std::cout << "Initialized Imgui." << std::endl;
}

/*void GLUTBackend::setOpenGLOutput(Graphics::OutputOpenGL *outputOpenGL) {
    this->outGL = outputOpenGL;

    IntPair size = outGL->getWindowSizeHint();

    if(size.first == -1 || size.second == -1) {
        std::cout << "/nWarning: using default window size fullscreen.";
        size = {800, 450};
        glutFullScreen();
    } else {
        glutReshapeWindow(size.first, size.second);
    }
}*/

auto GLUTBackend::GetInstance() -> GLUTBackend *{
    if(GLUTBackend::glutBackend == nullptr)
        GLUTBackend::glutBackend = new GLUTBackend();

    return GLUTBackend::glutBackend;
}

GLUTBackend::~GLUTBackend() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
};

void GLUTBackend::run(Program *pProgram)
{
    this->program = pProgram;

    glutMainLoop();
}

void GLUTBackend::keyboard(unsigned char key, int x, int y)
{
    GLUTBackend *me = GLUTBackend::GetInstance();
    Program *program = me->program;

    if(key == 27) {
        glutLeaveMainLoop();
    } else if(key == ' ') {
        me->programIsRunning = !me->programIsRunning;
    } else if(key == '=') {
        me->steps += 1;

        std::cout << "\nStepping " << me->steps << " per cycle.";
    } else if(key == '+') {
        me->steps *= 1.1;

        std::cout << "\nStepping " << me->steps << " per cycle.";
    } else if(key == '-') {
        if(me->steps > 1) {
            me->steps -= 1;

            if (me->steps <= 0) me->steps = 1;

            std::cout << "\nStepping " << me->steps << " per cycle.";
        }
    } else if(key == '_') {
        if(me->steps > 1) {
            me->steps /= 1.1;

            if (me->steps <= 0) me->steps = 1;
        }
    } else if(key == 'd') {
        me->showDemo = !me->showDemo;
    }
    else if(key == 'f') {
        dynamic_cast<NumericalIntegration*>(program)->doForceOverStepping();
    } else if(key == '[') {
        program->step(1);
    } else if(key == '{') {
        program->step(20);
    } /*else if (key == 16) { // glutGetModifiers() & GLUT_ACTIVE_CTRL && (key == 'p' || key == 'P') )
        auto buffer = GLUTUtils::getFrameBuffer();

        OpenGLUtils::outputToPNG(buffer, "beautiful_graphy-graph.png");
    } */
    else {
        auto *gb = GLUTBackend::GetInstance();
        for(auto &win : gb->windows)
            win->notifyKeyboard(key, x, y);
    }
}

void GLUTBackend::keyboardSpecial(int key, int x, int y)
{
    auto *gb = GLUTBackend::GetInstance();
    for(auto &win : gb->windows)
        win->notifyKeyboardSpecial(key, x, y);
}

void GLUTBackend::mouseButton(int button, int dir, int x, int y)
{
    if(ImGui::GetIO().WantCaptureMouse)
    {
        ImGui_ImplGLUT_MouseFunc(button, dir, x, y);
        return;
    }

    auto *gb = GLUTBackend::GetInstance();
    for(auto &win : gb->windows)
        win->notifyMouseButton(button, dir, x, y);

}

void GLUTBackend::mouseWheel(int wheel, int direction, int x, int y){
    auto *gb = GLUTBackend::GetInstance();
    for(auto &win : gb->windows)
        win->notifyMouseWheel(wheel, direction, x, y);
}

void GLUTBackend::mousePassiveMotion(int x, int y)
{
    {
        ImGui_ImplGLUT_MotionFunc(x, y);
        // if(ImGui::GetIO().WantCaptureMouse) return;
    }

    auto *gb = GLUTBackend::GetInstance();
    for(auto &win : gb->windows)
        win->notifyMousePassiveMotion(x, y);
}

void GLUTBackend::mouseMotion(int x, int y)
{
    {
        ImGui_ImplGLUT_MotionFunc(x, y);
        // if(ImGui::GetIO().WantCaptureMouse) return;
    }

    auto *gb = GLUTBackend::GetInstance();
    for(auto &win : gb->windows)
        win->notifyMouseMotion(x, y);
}

void GLUTBackend::render()
{
    GLUTBackend *gb = GLUTBackend::GetInstance();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();
    if(gb->showDemo) ImGui::ShowDemoWindow();

    for(auto &win : gb->windows) {
        //win->addStat(ToString(gb->steps) + " sim steps per cycle.");
        win->notifyRender();
    }


    ImGui::Render();
    //ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glutSwapBuffers();
    glutPostRedisplay();
}

void GLUTBackend::idleCall()
{
    GLUTBackend *gb = GLUTBackend::GetInstance();
    Program *program = gb->program;


    GLenum err;
    static GLenum lastErr = 0;
    while((err = glGetError()) != GL_NO_ERROR){
        if(lastErr == err) continue;

        std::cout << "OpenGL error " << err << std::endl;
        lastErr = err;
    }

    if(gb->isRunning()){
        int dummy = 1;
        int *dummy_ptr = &dummy;
        // dummy_ptr = nullptr;

        program->step(gb->steps, dummy_ptr);
    }
}

void GLUTBackend::reshape(int w, int h)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);

    GLUTBackend::GetInstance()->w = w;
    GLUTBackend::GetInstance()->h = h;

    GLUTBackend *gb = GLUTBackend::GetInstance();

    for(auto &win : gb->windows)
        win->notifyScreenReshape(w, h);

    glutPostRedisplay();
}

void GLUTBackend::addWindow(Window::Ptr window) {
    // this->outGL = outputOpenGL;

    //IntPair size = outGL->getWindowSizeHint();
    //
    //if(size.first == -1 || size.second == -1) {
    //    std::cout << "/nWarning: using default window size fullscreen.";
    //    size = {800, 450};
    //    glutFullScreen();
    //} else {
    //    glutReshapeWindow(size.first, size.second);
    //}

    windows.emplace_back(window);
}

