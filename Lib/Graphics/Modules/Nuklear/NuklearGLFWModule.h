//
// Created by joao on 28/09/23.
//

#ifndef STUDIOSLAB_NUKLEARGLFWMODULE_H
#define STUDIOSLAB_NUKLEARGLFWMODULE_H

#include "NuklearModule.h"

#include "3rdParty/NuklearSource.h"

#include "Graphics/Backend/GLFW/GLFWListener.h"
#include "Utils/Types.h"


namespace Slab::Graphics {

    class NuklearGLFWModule : public NuklearModule, public GLFWListener{
        GLFWwindow *renderWindow = nullptr;
        StrVector text;

    public:
        NuklearGLFWModule();
        ~NuklearGLFWModule() override;

        // void beginEvents() override;

        bool CharEvent(GLFWwindow *window, Codepoint value) override;

        bool MouseButton(GLFWwindow *window, int button, int action, int mods) override;

        bool MouseWheel(GLFWwindow *window, double xoffset, double yoffset) override;
    };

} // Core

#endif //STUDIOSLAB_NUKLEARGLFWMODULE_H
