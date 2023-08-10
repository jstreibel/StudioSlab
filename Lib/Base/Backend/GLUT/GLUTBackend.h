#ifndef GLUTBACKEND_H
#define GLUTBACKEND_H

#include "Base/Backend/Events/EventListener.h"
#include "Base/Backend/GUIBackend.h"
#include "Phys/Graph/OpenGLMonitor.h"



class GLUTBackend : public GUIBackend
{
	friend DerivableSingleton;

    GLUTBackend();
    ~GLUTBackend() override;

    int w, h;
	bool showDemo = false;
	bool programIsRunning = false;

	Program *program = nullptr;
	std::vector<Numerics::OutputSystem::Socket*> sockets;

public:
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

    auto isRunning() const -> bool {return programIsRunning;}
	void pause()  override { programIsRunning = false; }
	void resume() override { programIsRunning = true; }

};

#endif // GLUTBACKEND_H
