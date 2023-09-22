
#include "Core/Graphics/OpenGL/OpenGL.h"
#include <GL/freeglut.h>

#include "GLUTBackend.h"

#include "Core/Graphics/OpenGL/GLDebug.h"
#include "Core/Tools/Log.h"
#include "Core/Tools/Animator.h"

#include "Math/Graph/Styles.h"
#include "Math/Numerics/Program/Integrator.h"
#include "Core/Backend/BackendManager.h"

#include <cassert>
#include <filesystem>

#define FULLSCREEN true

GLUTBackend::GLUTBackend() : GUIBackend("GLUT backend") {
    int dummy = 0;
    glutInit(&dummy, nullptr);

    fix w = 3200, h = 1600;

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

    OpenGL::StartupDebugLogging();

    Log::Success() << "Initialized GLUTBackend. "
                   << "OpenGL version " << glGetString(GL_VERSION) << ". "
                   << "Current window: " << winHandle << Log::Flush;

    Styles::Init();

    Log::Info() << "Initialized Imgui." << Log::Flush;
}

GLUTBackend::~GLUTBackend() = default;

void GLUTBackend::run(Program *pProgram)
{
    this->program = pProgram;

    glutMainLoop();
}

void GLUTBackend::keyboard(unsigned char key, int x, int y)
{
    auto &me = GetInstance();

    if(key == 27) glutLeaveMainLoop();

    for(auto &module : me.getModules())
        if (module->notifyKeyboard(static_cast<Core::KeyMap>(key), Core::Press, {})) return;

    Program *program = me.program;

    if(key == ' ') me.programIsRunning = !me.programIsRunning;
    else if(key == 'f') dynamic_cast<NumericalIntegration*>(program)->doForceOverStepping();
    else if(key == '[') {
        Log::Info("GLUTBackend ") << "forcing cycle until next output;" << Log::Flush;
        program->cycle(Program::CycleOptions::CycleUntilOutput);
    } else for(auto &listener : me.listeners) listener->notifyKeyboard(static_cast<Core::KeyMap>(key), Core::Press, {});
}

void GLUTBackend::keyboardSpecial(int key, int x, int y)
{
    Core::KeyMap keyMap;
    switch (key) {
        case GLUT_KEY_NUM_LOCK: keyMap = Core::Key_NUM_LOCK; break;
        case GLUT_KEY_BEGIN:    keyMap = Core::Key_HOME; break;
        case GLUT_KEY_DELETE:   keyMap = Core::Key_DELETE; break;
        case GLUT_KEY_SHIFT_L:  keyMap = Core::Key_LEFT_SHIFT; break;
        case GLUT_KEY_SHIFT_R:  keyMap = Core::Key_RIGHT_SHIFT; break;
        case GLUT_KEY_CTRL_L:   keyMap = Core::Key_LEFT_CONTROL; break;
        case GLUT_KEY_CTRL_R:   keyMap = Core::Key_RIGHT_CONTROL; break;
        case GLUT_KEY_ALT_L:    keyMap = Core::Key_LEFT_ALT; break;
        case GLUT_KEY_ALT_R:    keyMap = Core::Key_RIGHT_ALT; break;
        case GLUT_KEY_SUPER_L:  keyMap = Core::Key_LEFT_SUPER; break;
        case GLUT_KEY_SUPER_R:  keyMap = Core::Key_RIGHT_SUPER; break;

        default: keyMap = Core::Key_UNKNOWN;
    }

    auto &me = GetInstance();

    for(auto &module : me.getModules())
        if (module->notifyKeyboard(keyMap, Core::Press, {})) return;

    for(auto &listener: me.listeners) listener->notifyKeyboard(keyMap, Core::Press, {});
}

void GLUTBackend::mouseButton(int button, int state, int x, int y)
{
    auto &mouseState = GetInstance().mouseState;

    mouseState.dx = x - mouseState.x;
    mouseState.dy = y - mouseState.y;
    mouseState.x = x;
    mouseState.y = y;
    if (button == GLUT_LEFT_BUTTON) mouseState.leftPressed = (state == GLUT_DOWN);
    else if (button == GLUT_MIDDLE_BUTTON) mouseState.centerPressed = (state == GLUT_DOWN);
    else if (button == GLUT_RIGHT_BUTTON) mouseState.rightPressed = (state == GLUT_DOWN);

    std::map<int, Core::MouseButton> mouseMap = {
        {GLUT_LEFT_BUTTON,   Core::MouseButton_LEFT   },
        {GLUT_MIDDLE_BUTTON, Core::MouseButton_MIDDLE },
        {GLUT_RIGHT_BUTTON,  Core::MouseButton_RIGHT  },
    };

    std::map<int, Core::KeyState> buttonStateMap = {
            {GLUT_UP,   Core::Release },
            {GLUT_DOWN, Core::Press   },
    };

    auto &me = GetInstance();

    for(auto &module : me.getModules())
        if (module->notifyMouseButton(mouseMap[button], buttonStateMap[state], {})) return;

    for (auto &listener: me.listeners)
        listener->notifyMouseButton(mouseMap[button], buttonStateMap[state], {});

}

void GLUTBackend::mouseWheel(int wheel, int direction, int x, int y){
    auto &me = GetInstance();

    auto dx = wheel==0 ? direction : 0;
    auto dy = wheel==1 ? direction : 0;

    for(auto &module : me.getModules())
        if (module->notifyMouseWheel(dx, dy)) return;

    for (auto &listener: me.listeners)
        listener->notifyMouseWheel(dx, dy);
}

void GLUTBackend::mousePassiveMotion(int x, int y)
{
    auto &me = GetInstance();
    {
        auto &mouseState = me.mouseState;
        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;
    }

    for (auto &module : me.getModules())
        if (module->notifyMouseMotion(x, y)) return;

    for (auto &listener: me.listeners)
        listener->notifyMouseMotion(x, y);
}

void GLUTBackend::mouseMotion(int x, int y)
{
    auto &me = GetInstance();
    {
        auto &mouseState = me.mouseState;
        mouseState.dx = x - mouseState.x;
        mouseState.dy = y - mouseState.y;
        mouseState.x = x;
        mouseState.y = y;
    }


    for (auto &module : me.getModules())
        if (module->notifyMouseMotion(x, y)) return;

    for (auto &listener: me.listeners)
        listener->notifyMouseMotion(x, y);
}

void GLUTBackend::render()
{
    auto &me = GetInstance();

    if(me.renderingRequested) me.renderingRequested = false;

    Core::Graphics::Animator::Update();

    auto bg = Core::Graphics::backgroundColor;
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto &modules = me.getModules();

    for(auto &module : modules)
        module->beginRender();

    for(auto &listener: me.listeners)
        listener->notifyRender();

    for(auto &module : modules)
        module->notifyRender();

    for(auto &module : modules)
        module->endRender();

    glutSwapBuffers();
    glutPostRedisplay();
}

void GLUTBackend::idleCall()
{
    GET me = GetInstance();
    Program *program = me.program;

    while(!me.isPaused() && !me.renderingRequested)
        if(!program->cycle(Program::CycleOptions::CycleUntilOutput)) break;
}

void GLUTBackend::reshape(int w, int h)
{
    auto &me = GetInstance();

    for(auto &module : GetInstance().getModules()) module->notifyScreenReshape(w, h);

    for(auto &listener: me.listeners) listener->notifyScreenReshape(w, h);

    glutPostRedisplay();
}

Real GLUTBackend::getScreenHeight() const { return GLUTUtils::getScreenHeight(); }

GLUTBackend &GLUTBackend::GetInstance() {
    assert(Core::BackendManager::GetImplementation() == Core::GLUT);

    auto &guiBackend = Core::BackendManager::GetGUIBackend();

    return *dynamic_cast<GLUTBackend*>(&guiBackend);
}

void GLUTBackend::requestRender() {
    renderingRequested = true;
}


