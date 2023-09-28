#ifndef GLUTBACKEND_H
#define GLUTBACKEND_H

#include "GLUTListener.h"
#include "GLUTEventTranslator.h"

#include "Core/Backend/Events/GUIEventListener.h"
#include "Core/Backend/GraphicBackend.h"

#include "Math/Graph/OpenGLMonitor.h"



class GLUTBackend : public GraphicBackend
{
    ::Backend::GLUTEventTranslator eventTranslator;
    std::vector<::Backend::GLUTListener*> glutListeners;

	bool programIsRunning = false;
	Program *program = nullptr;

	MouseState mouseState;
	bool renderingRequested = false;
public:
    GLUTBackend();
    ~GLUTBackend() override;

	static GLUTBackend &GetInstance();

    void run(Program *) override;

    static void keyboard(unsigned char key, int x, int y);
    static void keyboardUp(unsigned char key, int x, int y);
    static void keyboardSpecial(int key, int x, int y);
    static void keyboardSpecialUp(int key, int x, int y);

    static void mouseButton(int button, int dir, int x, int y);
    static void mousePassiveMotion(int x, int y);
    static void mouseWheel(int wheel, int direction, int x, int y);
    static void mouseMotion(int x, int y);

    static void render();
    static void idleCall();
    static void reshape(int w, int h);

    auto addGLUTListener(::Backend::GLUTListener *glutListener) -> void;

    auto getScreenHeight() const -> Real override;

    auto isPaused() const -> bool {return !programIsRunning;}
	void pause()  override { programIsRunning = false; }
	void resume() override { programIsRunning = true; }

	auto requestRender() -> void override;

	auto getMouseState() const -> MouseState override;

};

#endif // GLUTBACKEND_H
