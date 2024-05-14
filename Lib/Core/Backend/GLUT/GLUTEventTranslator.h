//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_GLUTEVENTTRANSLATOR_H
#define STUDIOSLAB_GLUTEVENTTRANSLATOR_H

#include "GLUTListener.h"

#include "Core/Backend/Events/GUIEventTranslator.h"
#include "Core/Backend/Events/MouseState.h"

namespace Backend {

    class GLUTEventTranslator : public Core::EventTranslator, public GLUTListener {
        MouseState mouseState;

    public:
        bool keyboard(unsigned char key, int x, int y) override;
        bool keyboardUp(unsigned char key, int x, int y) override;
        bool keyboardSpecial(int key, int x, int y) override;
        bool keyboardSpecialUp(int key, int x, int y) override;

        bool mouseButton(int button, int dir, int x, int y) override;
        bool mousePassiveMotion(int x, int y) override;
        bool mouseWheel(int wheel, int dir, int x, int y) override;
        bool mouseMotion(int x, int y) override;

        bool render() override;
        bool idle() override;

        const MouseState &getMouseState() const;

        bool reshape(int w, int h) override;
    };

} // Backend

#endif //STUDIOSLAB_GLUTEVENTTRANSLATOR_H
