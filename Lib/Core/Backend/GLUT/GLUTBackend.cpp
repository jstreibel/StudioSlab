
#include "Core/Graphics/OpenGL/OpenGL.h"
#include <GL/freeglut.h>

#include "GLUTBackend.h"

#include <Utils/Resources.h>

#include "3rdParty/imgui/imgui.h"
#include "3rdParty/imgui/backends/imgui_impl_glut.h"
#include "3rdParty/imgui/backends/imgui_impl_opengl3.h"
#include "3rdParty/ImGuiColorsSetup.h"

#include "Math/Numerics/Program/Integrator.h"
#include "Core/Graphics/Styles/StylesAndColorSchemes.h"
#include "Core/Graphics/OpenGL/Utils.h"
#include "Core/Tools/Log.h"
#include "3rdParty/glfreetype/TextRenderer.hpp"

#include <cassert>
#include <filesystem>

//#define FORCE_FPS 60
//const Real FRAME_TIME = 1.0/Real(FORCE_FPS);

#define FULLSCREEN true
fix IMGUI_FONT_INDEX = 10; //6;

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

    if(FULLSCREEN) glutFullScreen();

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


    Log::Success() << "Initialized GLUTBackend. Current window: " << winHandle << Log::Flush;


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

    {
        ImGui::GetStyle().WindowPadding = {20, 10};
        ImGui::GetStyle().FramePadding = ImVec2(12.0f,6.0f);
        ImGui::GetStyle().CellPadding = {9, 5};
        ImGui::GetStyle().ItemSpacing = ImVec2(17.0f,8.0f);
        ImGui::GetStyle().ItemInnerSpacing = {16, 8};
        ImGui::GetStyle().IndentSpacing = 30;
        ImGui::GetStyle().ScrollbarSize = 18;
        ImGui::GetStyle().GrabMinSize = 18;

        ImGui::GetStyle().WindowBorderSize = 1.0f;
        ImGui::GetStyle().FrameBorderSize = 0.0f;

        ImGui::GetStyle().WindowRounding = 4.0f;
        ImGui::GetStyle().ChildRounding = 5.0f;
        ImGui::GetStyle().FrameRounding = 5.0f;
        ImGui::GetStyle().ScrollbarRounding = 3.0f;
        ImGui::GetStyle().GrabRounding = 4.0f;
        ImGui::GetStyle().TabRounding = 5;
    }

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
        static const ImWchar ranges[] =
                {
                       0x0020, 0x007F, // Basic Latin
                       0x0391, 0x03C9, // Greek
                       0x2200, 0x22FF, // Mathematical operators
                       0x2A00, 0x2AFF, // Supplemental mathematical operators
                       // too big: 0x1D400, 0x1D7FF, // Mathematical alphanumeric symbols
                       0x00B0, 0x00BF, // Superscript / subscript
                       0x03D0, 0x03F6,
                       0x2070, 0x209F, // Superscript / subscript
                       0,
                };

        ImFontGlyphRangesBuilder glyphRangesBuilder;
        glyphRangesBuilder.AddRanges(ranges);
        if(0) for(ImWchar c: {0x00b2, 0x00b3, 0x00b9}) glyphRangesBuilder.AddChar(c);
        static ImVector<ImWchar> vRanges;
        glyphRangesBuilder.BuildRanges(&vRanges);

        auto &log = Log::Info() << "ImGui loading glyph ranges: ";
        int i=0;
        for(auto &v : vRanges){
            if(v==0) break;
            log << std::hex << v << (++i%2?"-":" ");
        }
        log << std::dec << Log::Flush;

        ImGuiIO& io = ImGui::GetIO();
        auto fontName = Resources::fontFileName(IMGUI_FONT_INDEX);

        if (!std::filesystem::exists(fontName)) throw Str("Font ") + fontName + " does not exist.";

        ImFontConfig fontConfig;
        auto font = io.Fonts->AddFontFromFileTTF(fontName.c_str(), 26.0f, &fontConfig, &vRanges[0]);

        io.FontDefault = font;

        Log::Info() << "ImGui using font '" << Resources::fonts[IMGUI_FONT_INDEX] << "'." << Log::Flush;

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
    if(key == 27) {
        glutLeaveMainLoop();
    }

    {
        ImGui_ImplGLUT_KeyboardFunc(key, x, y);
        if(ImGui::GetIO().WantCaptureKeyboard)
            return;
    }

    auto &me = GetInstanceSuper<GLUTBackend>();
    Program *program = me.program;

    if(key == ' ') {
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
    {
        ImGui_ImplGLUT_SpecialFunc(key, x, y);
        if(ImGui::GetIO().WantCaptureKeyboard)
            return;
    }

    auto &me = GetInstanceSuper<GLUTBackend>();
    for(auto &win : me.windows)
        win->notifyKeyboardSpecial(key, x, y);
}

void GLUTBackend::mouseButton(int button, int state, int x, int y)
{
    {
        auto &mouseState = GetInstanceSuper<GLUTBackend>().mouseState;

        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;
        if (button == GLUT_LEFT_BUTTON) mouseState.leftPressed = (state == GLUT_DOWN);
        else if (button == GLUT_MIDDLE_BUTTON) mouseState.centerPressed = (state == GLUT_DOWN);
        else if (button == GLUT_RIGHT_BUTTON) mouseState.rightPressed = (state == GLUT_DOWN);
    }

    {
        ImGui_ImplGLUT_MouseFunc(button, state, x, y);
        if(ImGui::GetIO().WantCaptureMouse)
            return;
    }

    {
        auto &me = GetInstanceSuper<GLUTBackend>();
        for (auto &win: me.windows)
            win->notifyMouseButton(button, state, x, y);
    }
}

void GLUTBackend::mouseWheel(int wheel, int direction, int x, int y){
    {
        ImGui_ImplGLUT_MouseWheelFunc(wheel, direction, x, y);
        if (ImGui::GetIO().WantCaptureMouse)
            return;
    }

    auto &me = GetInstanceSuper<GLUTBackend>();
    for (auto &win: me.windows)
        win->notifyMouseWheel(wheel, direction, x, y);
}

void GLUTBackend::mousePassiveMotion(int x, int y)
{
    auto &me = GetInstanceSuper<GLUTBackend>();

    {
        auto &mouseState = me.mouseState;
        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;
    }

    {
        ImGui_ImplGLUT_MotionFunc(x, y);
        if(ImGui::GetIO().WantCaptureMouse)
            return;
    }

    {
        for (auto &win: me.windows)
            win->notifyMousePassiveMotion(x, y);
    }
}

void GLUTBackend::mouseMotion(int x, int y)
{
    auto &me = GetInstanceSuper<GLUTBackend>();
    {
        auto &mouseState = me.mouseState;
        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;
    }

    {
        ImGui_ImplGLUT_MotionFunc(x, y);
        if(ImGui::GetIO().WantCaptureMouse)
            return;
    }

    {
        for (auto &win: me.windows)
            win->notifyMouseMotion(x, y);
    }
}

void GLUTBackend::render()
{
    auto &me = GetInstanceSuper<GLUTBackend>();

    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();
        if (me.showDemo) ImGui::ShowDemoWindow();
    }


    {
        static auto timer = Timer();
        for (auto &win: me.windows) {
            auto elapsed = timer.getElTime_msec();
            win->notifyRender((float)elapsed);
        }
        timer.reset();
    }

    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void GLUTBackend::idleCall()
{
    GET me = GetInstanceSuper<GLUTBackend>();
    Program *program = me.program;

    GLenum err;
    static GLenum lastErr = 0;
    while((err = glGetError()) != GL_NO_ERROR){
        if(lastErr == err) continue;

        Log::Warning() << "OpenGL error " << err << Log::Flush;
        lastErr = err;
    }

    while(!me.isPaused() && !me.renderingRequested())
        if(!program->cycle(Program::CycleOptions::CycleUntilOutput)) break;

}

void GLUTBackend::reshape(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float) w, (float) h);
    }

    auto &me = GetInstanceSuper<GLUTBackend>();

    me.w = w;
    me.h = h;

    for(auto &win : me.windows)
        win->notifyScreenReshape(w, h);

    glutPostRedisplay();
}

void GLUTBackend::addWindow(Window::Ptr window) {
    try {
        auto socket = dynamic_cast<Numerics::OutputSystem::Socket*>(window.get());
        sockets.emplace_back(socket);
    } catch (std::bad_cast &) { }

    GUIBackend::addWindow(window);
}


