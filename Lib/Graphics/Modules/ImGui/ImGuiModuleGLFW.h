//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULEGLFW_H
#define STUDIOSLAB_IMGUIMODULEGLFW_H

#include "ImGuiModule.h"
#include "Graphics/Backend/GLFW/GLFWListener.h"

namespace Slab::Graphics {

    class ImGuiModuleGLFW : public FImGuiModule
    {
    public:
        explicit ImGuiModuleGLFW(GLFWwindow*);
    };

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUIMODULEGLFW_H
