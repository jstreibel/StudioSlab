//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_GLFWEVENTTRANSLATOR_H
#define STUDIOSLAB_GLFWEVENTTRANSLATOR_H

#include "GLFWListener.h"

#include "Graphics/Backend/Events/SystemWindowEventTranslator.h"

namespace Slab::Graphics {
    class GLFWEventTranslator : public EventTranslator, public GLFWListener{
    public:
        GLFWEventTranslator();

        bool KeyboardEvent(GLFWwindow *window, int key, int scancode, int action, int mods) override;
        bool MouseMotion(GLFWwindow *window, double xpos, double ypos) override;
        void CursorEnter(GLFWwindow *window, int entered) override;
        bool MouseButton(GLFWwindow *window, int button, int action, int mods) override;
        bool MouseWheel(GLFWwindow *window, double xoffset, double yoffset) override;
        bool DroppedFiles(GLFWwindow *window, int count, const char **paths) override;

        void Render(GLFWwindow *window) override;

        void ScreenReshape(GLFWwindow *window, int width, int height) override;
    };

} // Core

#endif //STUDIOSLAB_GLFWEVENTTRANSLATOR_H
