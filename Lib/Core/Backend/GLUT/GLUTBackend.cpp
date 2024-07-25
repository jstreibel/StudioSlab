

#include "GLUTBackend.h"

#include "Core/Backend/BackendManager.h"
#include "Core/Tools/Log.h"
#include "Utils/ReferenceIterator.h"
// #include "GLUTUtils.h"

#include <GL/freeglut.h>
#include <cassert>

#define FULLSCREEN true

namespace Slab::Core {

    GLUTBackend::GLUTBackend()
            : GraphicBackend("GLUT backend", eventTranslator) {
        int dummy = 0;
        glutInit(&dummy, nullptr);

        fix w = 3200, h = 1600;

        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
        //glutInitWindowSize(w, h);
        glutInitWindowPosition(40, 200);
        glutInitWindowSize(w, h);
        int winHandle = glutCreateWindow("Pendulum");

        if (FULLSCREEN) glutFullScreen();

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

        addGLUTListener(eventTranslator);

        Log::Success() << "Initialized GLUTBackend. "
                       << "OpenGL version " << glGetString(GL_VERSION) << ". "
                       << "Current window: " << winHandle << Log::Flush;

        Log::Info() << "Initialized Imgui." << Log::Flush;
    }

    GLUTBackend::~GLUTBackend() = default;

    void GLUTBackend::run() {
        glutMainLoop();
    }

    void GLUTBackend::keyboard(unsigned char key, int x, int y) {
        auto &me = GetInstance();

        if (key == 27) glutLeaveMainLoop();

        IterateReferences(me.glutListeners,
                          Func(keyboard, key, x, y),
                          StopOnFirstResponder);
    }

    void GLUTBackend::keyboardUp(unsigned char key, int x, int y) {
        auto &me = GetInstance();

        IterateReferences(me.glutListeners, Func(keyboardUp, key, x, y), StopOnFirstResponder);
    }

    void GLUTBackend::keyboardSpecial(int key, int x, int y) {
        auto &me = GetInstance();

        IterateReferences(me.glutListeners,Func(keyboardSpecial,key, x, y), StopOnFirstResponder);
    }

    void GLUTBackend::keyboardSpecialUp(int key, int x, int y) {
        auto &me = GetInstance();

        IterateReferences(me.glutListeners, Func(keyboardSpecialUp, key, x, y), IterateAll);
    }

    void GLUTBackend::mouseButton(int button, int state, int x, int y) {
        auto &me = GetInstance();

        IterateReferences(me.glutListeners, Func(mouseButton, button, state, x, y), IterateAll);
    }

    void GLUTBackend::mouseWheel(int wheel, int direction, int x, int y) {
        auto &me = GetInstance();

        IterateReferences(me.glutListeners, Func(mouseWheel, wheel, direction, x, y), StopOnFirstResponder);
    }

    void GLUTBackend::mousePassiveMotion(int x, int y) {
        auto &me = GetInstance();
        {
            auto &mouseState = me.mouseState;
            mouseState.dx = x - mouseState.x;
            mouseState.dy = y - mouseState.y;
            mouseState.x = x;
            mouseState.y = y;
        }

        IterateReferences(me.glutListeners, Func(mousePassiveMotion, x, y), IterateAll);
    }

    void GLUTBackend::mouseMotion(int x, int y) {
        auto &me = GetInstance();

        IterateReferences(me.glutListeners, Func(mouseMotion, x, y));
    }

    void GLUTBackend::render() {
        auto &me = GetInstance();

        glClearColor(me.r, me.g, me.b, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto modules = me.getGraphicsModules();

        IterateReferences(modules, FuncRun(beginRender));
        IterateReferences(me.glutListeners, FuncRun(render));

        IterateReferences(modules, FuncRun(endRender));

        glutSwapBuffers();
        glutPostRedisplay();
    }

    void GLUTBackend::idleCall() {
        GET me = GetInstance();

        IterateReferences(me.glutListeners, FuncRun(idle));
    }

    void GLUTBackend::reshape(int w, int h) {
        auto &me = GetInstance();

        IterateReferences(me.glutListeners, FuncRun(reshape, w, h));

        glutPostRedisplay();
    }

    Real GLUTBackend::getScreenHeight() const {
        return glutGet(GLUT_WINDOW_HEIGHT);
    }

    GLUTBackend &GLUTBackend::GetInstance() {
        //assert(Core::BackendManager::GetImplementation() == Core::GLUT);

        auto &guiBackend = Core::BackendManager::GetGUIBackend();

        return *dynamic_cast<GLUTBackend *>(&guiBackend);
    }

    MouseState GLUTBackend::getMouseState() const { return eventTranslator->getMouseState(); }

    auto GLUTBackend::addGLUTListener(Volatile<GLUTListener> glutListener) -> void {
        glutListeners.emplace_back(glutListener);
    }

    void GLUTBackend::terminate() {
        glutLeaveMainLoop();
    }

}