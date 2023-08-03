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
#include "Common/Log/Log.h"

#include <cassert>
#include <filesystem>

//#define FORCE_FPS 60
//const Real FRAME_TIME = 1.0/Real(FORCE_FPS);

GLUTBackend::GLUTBackend() : GUIBackend("GLUT backend") {
    assert(Backend::singleInstance == nullptr);

    int dummy = 0;
    glutInit(&dummy, nullptr);

    w = 3200, h = 1600;

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    //glutInitWindowSize(w, h);
    glutInitWindowPosition(40, 200);
    glutInitWindowSize(w, h);
    int winHandle = glutCreateWindow("Pendulum");

    if(false) glutFullScreen();

    // GLEW init:
    {
            GLenum initStatus = glewInit();
        if (GLEW_OK != initStatus) {
            char buffer[1024];
            sprintf(buffer, "Error: %s", glewGetErrorString(initStatus));
            throw Str(buffer);
        } else {
            char buffer[1024];
            sprintf(buffer, "Using GLEW %s", glewGetString(GLEW_VERSION));
            Log::Info() << buffer << Log::Flush;
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


    Log::Critical() << "Initialized GLUTBackend. Current window: " << winHandle << Log::Flush;


    // Setup Dear ImGui context
    // Themes: https://github.com/ocornut/imgui/issues/707

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    //SetupImGuiColors_BlackAndWhite();

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

        if (!std::filesystem::exists(fontName)) throw Str("Font ") + fontName + " does not exist.";

        auto font = io.Fonts->AddFontFromFileTTF(fontName.c_str(), 22.0f);
        io.FontDefault = font;

        //ImGui::PushFont(font);
    }

    ImGui::GetStyle().ScaleAllSizes(1.5);
    ImGui::GetIO().FontGlobalScale = 1;

    Styles::Init();

    Log::Info() << "Initialized Imgui." << Log::Flush;
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
    GLUTBackend &me = GetInstanceSuper<GLUTBackend>();
    Program *program = me.program;

    if(key == 27) {
        glutLeaveMainLoop();
    } else if(key == ' ') {
        me.programIsRunning = !me.programIsRunning;
    } else if(key == 'd') {
        me.showDemo = !me.showDemo;
    }
    else if(key == 'f') {
        dynamic_cast<NumericalIntegration*>(program)->doForceOverStepping();
    } else if(key == '[') {
        Log::Info("GLUTBackend ") << "forcing cycle until next output;" << Log::Flush;
        program->cycle(Program::CycleOptions::CycleUntilOutput);
    } else {
        for(auto &win : me.windows)
            win->notifyKeyboard(key, x, y);
    }
}

void GLUTBackend::keyboardSpecial(int key, int x, int y)
{
    GLUTBackend &me = GetInstanceSuper<GLUTBackend>();
    for(auto &win : me.windows)
        win->notifyKeyboardSpecial(key, x, y);
}

void GLUTBackend::mouseButton(int button, int state, int x, int y)
{
    auto &mouseState = GetInstanceSuper<GLUTBackend>().mouseState;

    mouseState.dx = x-mouseState.x;
    mouseState.dy = y-mouseState.y;
    mouseState.x = x;
    mouseState.y = y;
    if      (button == GLUT_LEFT_BUTTON)   mouseState.leftPressed   = (state == GLUT_DOWN);
    else if (button == GLUT_MIDDLE_BUTTON) mouseState.centerPressed = (state == GLUT_DOWN);
    else if (button == GLUT_RIGHT_BUTTON)  mouseState.rightPressed  = (state == GLUT_DOWN);

    // if(ImGui::GetIO().WantCaptureMouse)
    {
        ImGui_ImplGLUT_MouseFunc(button, state, x, y);
    }

    GLUTBackend &me = GetInstanceSuper<GLUTBackend>();
    for(auto &win : me.windows)
        win->notifyMouseButton(button, state, x, y);


}

void GLUTBackend::mouseWheel(int wheel, int direction, int x, int y){
    GLUTBackend &me = GetInstanceSuper<GLUTBackend>();
    for(auto &win : me.windows)
        win->notifyMouseWheel(wheel, direction, x, y);
}

void GLUTBackend::mousePassiveMotion(int x, int y)
{
    GLUTBackend &me = GetInstanceSuper<GLUTBackend>();
    auto &mouseState = me.mouseState;
    mouseState.dx = x-mouseState.x;
    mouseState.dy = y-mouseState.y;
    mouseState.x = x;
    mouseState.y = y;

    if(ImGui::GetIO().WantCaptureMouse)
    {
        ImGui_ImplGLUT_MotionFunc(x, y);
        return;
    }

    for(auto &win : me.windows)
        win->notifyMousePassiveMotion(x, y);
}

void GLUTBackend::mouseMotion(int x, int y)
{
    GLUTBackend &me = GetInstanceSuper<GLUTBackend>();
    auto &mouseState = me.mouseState;
    mouseState.dx = x-mouseState.x;
    mouseState.dy = y-mouseState.y;
    mouseState.x = x;
    mouseState.y = y;

    if(ImGui::GetIO().WantCaptureMouse)
    {
        ImGui_ImplGLUT_MotionFunc(x, y);
        return;
    }

    for(auto &win : me.windows)
        win->notifyMouseMotion(x, y);
}

void GLUTBackend::render()
{
    GLUTBackend &me = GetInstanceSuper<GLUTBackend>();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();
    if(me.showDemo) ImGui::ShowDemoWindow();

    static auto timer = Timer();

    for(auto &win : me.windows) {
        //win->addStat(ToString(gb->steps) + " sim steps per cycle.");
        auto elapsed = timer.getElTime_msec();
        win->notifyRender(elapsed);
    }
    timer.reset();


    ImGui::Render();
    //ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glutSwapBuffers();
    glutPostRedisplay();
}

void GLUTBackend::idleCall()
{
    GLUTBackend &me = GetInstanceSuper<GLUTBackend>();
    Program *program = me.program;


    GLenum err;
    static GLenum lastErr = 0;
    while((err = glGetError()) != GL_NO_ERROR){
        if(lastErr == err) continue;

        Log::Warning() << "OpenGL error " << err << Log::Flush;
        lastErr = err;
    }

    if(me.isRunning()){
        program->cycle(Program::CycleOptions::CycleUntilOutput);
    }
}

void GLUTBackend::reshape(int w, int h)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);

    GLUTBackend &me = GetInstanceSuper<GLUTBackend>();

    me.w = w;
    me.h = h;

    for(auto &win : me.windows)
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

    try {
        auto socket = dynamic_cast<Numerics::OutputSystem::Socket*>(window.get());
        sockets.emplace_back(socket);
    } catch (std::bad_cast &) { }

    GUIBackend::addWindow(window);
}


