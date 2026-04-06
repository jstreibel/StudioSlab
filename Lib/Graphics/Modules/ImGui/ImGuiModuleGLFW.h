//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULEGLFW_H
#define STUDIOSLAB_IMGUIMODULEGLFW_H

#include "ImGuiModule.h"
#include "Graphics/Backend/GLFW/GLFWListener.h"

namespace Slab::Graphics {

    class FImGuiModuleGLFW : public FImGuiModule
    {
    public:
        explicit FImGuiModuleGLFW(GLFWwindow*);
    };

    using ImGuiModuleGLFW [[deprecated("Use FImGuiModuleGLFW")]] = FImGuiModuleGLFW;

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUIMODULEGLFW_H
