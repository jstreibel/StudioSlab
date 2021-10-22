#include "GLUTBackend.h"

#include "GL/freeglut.h"

GLUTBackend *GLUTBackend::glutBackend = nullptr;

#define FORCE_FPS 60
const double FRAME_TIME = 1.0/double(FORCE_FPS);

GLUTBackend::GLUTBackend() : Backend(this)
{
    assert(GLUTBackend::glutBackend == nullptr);

    int dummy = 0;
    glutInit(&dummy, nullptr);

    w = 800, h = 600;

    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    //glutInitWindowSize(w, h);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Pendulum");
    glutFullScreen();
    glutDisplayFunc(GLUTBackend::render);
    glutReshapeFunc(GLUTBackend::reshape);
    glutKeyboardFunc(GLUTBackend::keyboard);
    glutMouseFunc(GLUTBackend::mouseButton);
    glutPassiveMotionFunc(GLUTBackend::mousePassiveMotion);
    glutMotionFunc(GLUTBackend::mouseMotion);
    glutSpecialFunc(GLUTBackend::keyboardSpecial);
    glutIdleFunc(GLUTBackend::idleCall);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_CONTINUE_EXECUTION);

    std::cout << "Initialized GLUTBackend." << std::endl;
}

void GLUTBackend::setOpenGLOutput(Base::OutputOpenGL *outputOpenGL) {
    this->outGL = outputOpenGL;

    IntPair size = outGL->getWindowSizeHint();

    glutReshapeWindow(size.first, size.second);
}

auto GLUTBackend::GetInstance() -> GLUTBackend *{
    if(GLUTBackend::glutBackend == nullptr)
        GLUTBackend::glutBackend = new GLUTBackend();

    return GLUTBackend::glutBackend;
}

GLUTBackend::~GLUTBackend() = default;

void GLUTBackend::run(Program *program)
{
    this->program = program;

    glutMainLoop();
}

void GLUTBackend::keyboard(unsigned char key, int x, int y)
{
    GLUTBackend *me = GLUTBackend::GetInstance();

    if(key == 27) glutLeaveMainLoop();
    else if(key == ' ') me->programIsRunning = !me->programIsRunning;
    else if(key == '=' || key == '+'){
        me->steps++;
    }
    else if(key == '-'){
        if(me->steps > 1) me->steps--;
    }
    /*else if(key == ']'){
        program->step(1);
    }
    else if(key == '}'){
        program->step(20);
    }*/
    else if(key == 'h'){
        //std::cout << me->program->getHistogram();
        /*
            if(sim == nullptr) return;
            // snapshot
            std::ostringstream buffer;
            buffer.setf(ios::fixed,ios::floatfield);
            buffer.precision(4);
            buffer << sim->getInfoString();

            OutputSnapshot::doOutput(sim->getOutputInfo(), buffer.str().c_str(), 4);
            */
    }
    else {
        GLUTBackend::GetInstance()->outGL->notifyGLUTKeyboard(key, x, y);
    }
}

void GLUTBackend::keyboardSpecial(int key, int x, int y)
{
    auto *outGL = GLUTBackend::GetInstance()->outGL;

    outGL->notifyGLUTKeyboardSpecial(key, x, y);

}

void GLUTBackend::mouseButton(int button, int dir, int x, int y)
{
    GLUTBackend *gb = GLUTBackend::GetInstance();
    auto *outGL = gb->outGL;

    outGL->notifyGLUTMouseButton(button, dir, x, y);

    glutPostRedisplay();
}

void GLUTBackend::mousePassiveMotion(int x, int y)
{

}

void GLUTBackend::mouseMotion(int x, int y)
{
    auto *outGL = GLUTBackend::GetInstance()->outGL;
    outGL->notifyGLUTMouseMotion(x, y);
}

void GLUTBackend::render()
{
    GLUTBackend *gb = GLUTBackend::GetInstance();
    auto *outGL = gb->outGL;

    StringStream cycles;
    cycles << "Stepping " << gb->steps << "/cycle." << std::endl;
    outGL->addVolatileStat(cycles.str());

    assert(outGL != nullptr);

    if(!gb->programIsRunning){
        outGL->finishFrameAndRender();
    }

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


    if(gb->programIsRunning){
        program->step(gb->steps);
    }
}

void GLUTBackend::reshape(int w, int h)
{
    GLUTBackend::GetInstance()->w = w;
    GLUTBackend::GetInstance()->h = h;

    GLUTBackend *gb = GLUTBackend::GetInstance();
    auto *outGL = gb->outGL;

    if(outGL != nullptr)
        outGL->reshape(w, h);
}

auto GLUTBackend::getWidth() -> int {
    return GLUTBackend::GetInstance()->w;
}

auto GLUTBackend::getHeight() -> int {
    return GLUTBackend::GetInstance()->h;
}

auto GLUTBackend::getProgram() const -> const Program & {
    return *program;
}

