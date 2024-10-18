#ifndef GLUTBACKEND_H
#define GLUTBACKEND_H

#include "GLUTListener.h"
#include "GLUTEventTranslator.h"

#include "Graphics/Backend/Events/GUIEventListener.h"
#include "Graphics/Backend/GraphicBackend.h"

namespace Slab::Graphics {

    class GLUTBackend : public GraphicBackend {
        Pointer<GLUTEventTranslator> eventTranslator;
        Vector<Volatile<GLUTListener>> glutListeners;

        MouseState mouseState;
    public:
        GLUTBackend();

        ~GLUTBackend() override;

        static GLUTBackend &GetInstance();

        void run() override;

        void terminate() override;

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

        auto addGLUTListener(Volatile<GLUTListener> glutListener) -> void;

        auto getScreenHeight() const -> Real override;

        auto getMouseState() const -> MouseState override;

    };

}
#endif // GLUTBACKEND_H
