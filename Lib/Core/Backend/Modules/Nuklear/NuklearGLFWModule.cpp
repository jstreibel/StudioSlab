//
// Created by joao on 28/09/23.
//


#include "3rdParty/Nuklear/NuklearSource.h"
#define NK_GLFW_GL4_IMPLEMENTATION
#include "Graphics/OpenGL/OpenGL.h"
#include "3rdParty/Nuklear/nuklear_glfw_gl4.h"

#include "NuklearGLFWModule.h"

#include "Core/Backend/GLFW/GLFWBackend.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Tools/Log.h"

#include "Utils/Resources.h"

#define MAX_VERTEX_BUFFER  (1024 * 1024)
#define MAX_ELEMENT_BUFFER (1024 * 1024)

namespace Core {

    NuklearGLFWModule::NuklearGLFWModule() : NuklearModule(GLFW) {
        try {
            auto &glfwBackend = dynamic_cast<GLFWBackend&>(BackendManager::GetGUIBackend());
            renderWindow = &glfwBackend.getGLFWWindow();

            glfwBackend.addGLFWListener(this);
        } catch (std::bad_cast& e) {
            Log::Error() << "Trying to instantiate Nuklear SFML module, but backend doesn't seem "
                            "to be SFML." << Log::Flush;
            return;
        }

        nkContext = nk_glfw3_init(renderWindow,
                                  NK_GLFW3_DEFAULT,
                                  MAX_VERTEX_BUFFER,
                                  MAX_ELEMENT_BUFFER);

        /* Load Fonts: if none of these are loaded a default font will be used  */
        /* Load Cursor: if you uncomment cursor loading please hide the cursor */
        {
            struct nk_font_atlas *atlas;
            nk_glfw3_font_stash_begin(&atlas);
            auto fontFolder = Resources::FontsFolder;
            struct nk_font *droid =     nk_font_atlas_add_from_file(atlas, (fontFolder + "imgui/DroidSans.ttf").c_str(), 24, 0);
            // struct nk_font *roboto =    nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);
            // struct nk_font *future =    nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);
            // struct nk_font *clean =     nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);
            // struct nk_font *tiny =      nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);
            // struct nk_font *cousine =   nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);
            nk_glfw3_font_stash_end();
            /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
            nk_style_set_font(nkContext, &droid->handle);
        }
    }

    NuklearGLFWModule::~NuklearGLFWModule() { nk_glfw3_shutdown(); }

    void NuklearGLFWModule::endRender() { nk_glfw3_render(NK_ANTI_ALIASING_ON); }

    void NuklearGLFWModule::beginEvents() {
        nk_glfw3_new_frame();
    }


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