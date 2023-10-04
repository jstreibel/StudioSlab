

#include "GLUTBackend.h"

#include "Core/Backend/BackendManager.h"
#include "Core/Tools/Log.h"
#include "Graphics/Styles/WindowStyles.h"
#include "Graphics/OpenGL/GLUTUtils.h"

#include <GL/freeglut.h>
#include <cassert>
#include <filesystem>

#define FULLSCREEN true

GLUTBackend::GLUTBackend()
: GraphicBackend("GLUT backend", eventTranslator)
{
    int dummy = 0;
    glutInit(&dummy, nullptr);

    fix w = 3200, h = 1600;

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    //glutInitWindowSize(w, h);
    glutInitWindowPosition(40, 200);
    glutInitWindowSize(w, h);
    int winHandle = glutCreateWindow("Pendulum");

    if(FULLSCREEN) glutFullScreen();

    glutDisplayFunc(GLUTBackend::render);
    glutReshapeFunc(GLUTBackend::reshape);

    glutKeyboardFunc(GLUTBackend::keyboard);
    glutKeyboardUpFunc(GLUTBackend::keyboardUp);
    glutSpecialFunc(GLUTBackend::keyboardSpecial);
    glutSpecialUpFunc(GLUTBackend::keyboardSpecialUp);

    glutMouseFunc(GLUTBackend::mouseButton);
    glutMouseWheelFunc(GLUTBackend::mouseWheel);
    glutPassiveMotionFunc(GLUTBackend::mousePassiveMotion);
    glutMotionFunc(GLUTBackend::mouseMotion);


    glutIdleFunc(GLUTBackend::idleCall);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    addGLUTListener(&eventTranslator);

    Log::Success() << "Initialized GLUTBackend. "
                   << "OpenGL version " << glGetString(GL_VERSION) << ". "
                   << "Current window: " << winHandle << Log::Flush;

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

    Program *program = me.program;

    if(key == ' ') me.programIsRunning = !me.programIsRunning;
    else if(key == '[') {
        Log::Info("GLUTBackend ") << "forcing cycle until next output;" << Log::Flush;
        program->cycle(Program::CycleOptions::CycleUntilOutput);
    } else {
        for(auto &listener : me.glutListeners) {
            if(listener->keyboard(key, x, y)) return;
        }
    }
}

void GLUTBackend::keyboardUp(unsigned char key, int x, int y) {
    auto &me = GetInstance();

    for(auto &listener : me.glutListeners) {
        if(listener->keyboardUp(key, x, y)) return;
    }
}

void GLUTBackend::keyboardSpecial(int key, int x, int y)
{
    auto &me = GetInstance();

    for(auto &listener : me.glutListeners)
        if(listener->keyboardSpecial(key, x, y)) return;
}

void GLUTBackend::keyboardSpecialUp(int key, int x, int y) {
    auto &me = GetInstance();

    for(auto &listener : me.glutListeners)
        if(listener->keyboardSpecialUp(key, x, y)) return;
}

void GLUTBackend::mouseButton(int button, int state, int x, int y)
{
    auto &me = GetInstance();

    for(auto &listener : me.glutListeners)
        if(listener->mouseButton(button, state, x, y))
            return;
}

void GLUTBackend::mouseWheel(int wheel, int direction, int x, int y){
    auto &me = GetInstance();

    for(auto &listener : me.glutListeners)
        if(listener->mouseWheel(wheel, direction, x, y)) return;
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

    for(auto &listener : me.glutListeners)
        if(listener->mousePassiveMotion(x, y)) return;

}

void GLUTBackend::mouseMotion(int x, int y)
{
    auto &me = GetInstance();

    for(auto &listener : me.glutListeners)
        if(listener->mouseMotion(x, y)) return;
}

void GLUTBackend::render()
{
    auto &me = GetInstance();

    if(me.renderingRequested) me.renderingRequested = false;

    auto bg = Graphics::clearColor;
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto &modules = me.getModules();
    for(auto &module : modules)
        module->beginRender();

    for(auto &listener : me.glutListeners)
        if(listener->render()) break;

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

    for(auto &listener: me.glutListeners) listener->reshape(w, h);

    glutPostRedisplay();
}

Real GLUTBackend::getScreenHeight() const { return Graphics::OpenGL::GLUT::getScreenHeight(); }

GLUTBackend &GLUTBackend::GetInstance() {
    assert(Core::BackendManager::GetImplementation() == Core::GLUT);

    auto &guiBackend = Core::BackendManager::GetGUIBackend();

    return *dynamic_cast<GLUTBackend*>(&guiBackend);
}

void GLUTBackend::requestRender() { renderingRequested = true; }

MouseState GLUTBackend::getMouseState() const { return eventTranslator.getMouseState(); }

auto GLUTBackend::addGLUTListener(::Backend::GLUTListener *glutListener) -> void {
    glutListeners.emplace_back(glutListener);
}

void GLUTBackend::finish() {
    glutLeaveMainLoop();
}
