#ifndef GLUTBACKEND_H
#define GLUTBACKEND_H

#include "GLUTEventListener.h"
#include "Studios/Backend/Backend.h"
#include "Studios/Graphics/ARCHIVE/NotHere/OutputOpenGL.h"

class GLUTBackend : public Backend
{
    static GLUTBackend *glutBackend;
    GLUTBackend();
    ~GLUTBackend() override;

    int w, h;

public:
    static GLUTBackend *GetInstance();

    void setOpenGLOutput(Base::OutputOpenGL *outputOpenGL);


    void run(Program *) override;

    static void keyboard(unsigned char key, int x, int y);
    static void keyboardSpecial(int key, int x, int y);

    static void mouseButton(int button, int dir, int x, int y);
    static void mousePassiveMotion(int x, int y);
    static void mouseWheel(int wheel, int direction, int x, int y);
    static void mouseMotion(int x, int y);

    static void render();
    static void idleCall();
    static void reshape(int w, int h);

    auto isRunning() const -> bool {return programIsRunning;}

	void setStepsPerFrame(size_t spf){ this->steps = spf; }

private:
	Program *program = nullptr;
    Base::OutputOpenGL *outGL = nullptr;

    int steps = 1;

    bool programIsRunning = false;

};

#endif // GLUTBACKEND_H
