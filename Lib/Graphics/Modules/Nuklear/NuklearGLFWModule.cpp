//
// Created by joao on 28/09/23.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "3rdParty/NuklearSource.h"
#define NK_GLFW_GL4_IMPLEMENTATION
#include "3rdParty/Nuklear/nuklear_glfw_gl4.h"

#include "NuklearGLFWModule.h"

#include "Graphics/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Tools/Log.h"

#include "Core/Tools/Resources.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/Backend/GLFW/GLFWSystemWindow.h"

#define MAX_VERTEX_BUFFER  (1024 * 1024)
#define MAX_ELEMENT_BUFFER (1024 * 1024)

namespace Slab::Graphics {

    const bool HighPriority = true;

    NuklearGLFWModule::NuklearGLFWModule() : NuklearModule(nullptr) {

    }

    NuklearGLFWModule::~NuklearGLFWModule() { nk_glfw3_shutdown(); }

    // void NuklearGLFWModule::endRender() { nk_glfw3_render(NK_ANTI_ALIASING_ON); }
    // void NuklearGLFWModule::beginEvents() { nk_glfw3_new_frame(); }


    bool NuklearGLFWModule::CharEvent(GLFWwindow *window, Codepoint value) {
        nk_glfw3_char_callback(window, value); return false;
    }

    bool NuklearGLFWModule::MouseButton(GLFWwindow *window, int button, int action, int mods) {
        nk_glfw3_mouse_button_callback(window, button, action, mods); return false;
    }

    bool NuklearGLFWModule::MouseWheel(GLFWwindow *window, double xoffset, double yoffset) {
        nk_gflw3_scroll_callback(window, xoffset, yoffset); return false;
    }

} // Core