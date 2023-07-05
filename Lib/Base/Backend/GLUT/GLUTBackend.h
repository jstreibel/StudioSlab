#ifndef GLUTBACKEND_H
#define GLUTBACKEND_H

#include "Base/Backend/Events/EventListener.h"
#include "Base/Backend/Backend.h"
#include "Phys/Graph/OutputOpenGL.h"

#include "../Events/MouseState.h"

class GLUTBackend : public Backend
{
    static GLUTBackend *glutBackend;
    GLUTBackend();
    ~GLUTBackend() override;

    int w, h;
	bool showDemo = false;
	bool programIsRunning = false;

	MouseState lastMouseState;
	MouseState mouseState;

	Program *program = nullptr;
	std::vector<Window::Ptr> windows;
	std::vector<Numerics::OutputSystem::Socket*> sockets;

public:
    static GLUTBackend *GetInstance();

	void addWindow(Window::Ptr window);

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

	auto getMouseState() const -> const MouseState&;

    auto isRunning() const -> bool {return programIsRunning;}
	void pause()  { programIsRunning = false; }
	void resume() { programIsRunning = true; }

};

#endif // GLUTBACKEND_H
