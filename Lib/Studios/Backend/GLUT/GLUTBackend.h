#ifndef GLUTBACKEND_H
#define GLUTBACKEND_H

#include "GLUTEventListener.h"
#include "Studios/Backend/Backend.h"

class GLUTBackend : public Backend
{
    static GLUTBackend *glutBackend;
    GLUTBackend();
    ~GLUTBackend() override;

    int w, h;

public:
    static GLUTBackend *GetInstance();

    void setOpenGLOutput(Base::GLUTEventListener *outputOpenGL);


    void run(Program *) override;

    static void keyboard(unsigned char key, int x, int y);
    static void keyboardSpecial(int key, int x, int y);
    static void mouseButton(int button, int dir, int x, int y);
    static void mousePassiveMotion(int x, int y);
    static void mouseMotion(int x, int y);
    static void render();
    static void idleCall();
    static void reshape(int w, int h);

    auto isRunning() const -> bool {return programIsRunning;}

private:
	Program *program = nullptr;
    Base::GLUTEventListener *outGL = nullptr;

    int steps = 10;

    bool programIsRunning = false;

};

#endif // GLUTBACKEND_H
