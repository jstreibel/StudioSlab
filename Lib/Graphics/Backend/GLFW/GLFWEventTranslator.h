//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_GLFWEVENTTRANSLATOR_H
#define STUDIOSLAB_GLFWEVENTTRANSLATOR_H

#include "GLFWListener.h"

#include "Graphics/Backend/Events/SystemWindowEventTranslator.h"

namespace Slab::Graphics {
    class FGLFWEventTranslator final : public FEventTranslator, public FGLFWListener{
    public:
        FGLFWEventTranslator();

        bool KeyboardEvent(GLFWwindow *, int key, int scancode, int action, int mods) override;
        bool CharEvent(GLFWwindow*, UInt codepoint) override;
        bool MouseMotion(GLFWwindow *, double xpos, double ypos) override;
        void CursorEnter(GLFWwindow *, int entered) override;
        bool MouseButton(GLFWwindow *, int button, int action, int mods) override;
        bool MouseWheel(GLFWwindow *, double xoffset, double yoffset) override;
        bool DroppedFiles(GLFWwindow *, int count, const char **paths) override;

        void Render(GLFWwindow *window) override;

        void ScreenReshape(GLFWwindow *window, int width, int height) override;
    };

} // Core

#endif //STUDIOSLAB_GLFWEVENTTRANSLATOR_H
