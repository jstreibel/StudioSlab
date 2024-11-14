//
// Created by joao on 29/09/23.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "NuklearTests.h"
#include "Core/Backend/BackendManager.h"

#include "Graphics/Modules/Nuklear/NuklearModule.h"
#include "3rdParty/Nuklear/nuklear_glfw_gl4.h"

#define NK_ASSERT(a) assert((a))

#include "3rdParty/Nuklear/common/style.c"
#include "3rdParty/Nuklear/common/calculator.c"
#include "3rdParty/Nuklear/common/canvas.c"
#include "3rdParty/Nuklear/common/overview.c"
#include "3rdParty/Nuklear/common/node_editor.c"
#include "3rdParty/Nuklear/common/file_browser.c"
#include "Core/Tools/Resources.h"
#include "Core/SlabCore.h"

namespace Tests {
    using namespace Slab;


    file_browser browser;
    media media;

    NuklearTests::NuklearTests() {
        Core::LoadModule("Nuklear");

        {
            glEnable(GL_TEXTURE_2D);

            auto baseFolder = Core::Resources::IconsFolder + "/nuklear/";

            media.icons.home =          icon_load((baseFolder + "home.png").c_str());
            media.icons.directory =     icon_load((baseFolder + "directory.png").c_str());
            media.icons.computer =      icon_load((baseFolder + "computer.png").c_str());
            media.icons.desktop =       icon_load((baseFolder + "desktop.png").c_str());
            media.icons.default_file =  icon_load((baseFolder + "default.png").c_str());
            media.icons.text_file =     icon_load((baseFolder + "text.png").c_str());
            media.icons.music_file =    icon_load((baseFolder + "music.png").c_str());
            media.icons.font_file =     icon_load((baseFolder + "font.png").c_str());
            media.icons.img_file =      icon_load((baseFolder + "img.png").c_str());
            media.icons.movie_file =    icon_load((baseFolder + "movie.png").c_str());
            media_init(&media);

            file_browser_init(&browser, &media);
        }
    }

    bool NuklearTests::notifyRender() {
        auto& nkModule = dynamic_cast<Graphics::NuklearModule&>(*Core::BackendManager::GetModule("Nuklear"));
        auto *nkContext =  nkModule.getContext();

        /* GUI */
        struct nk_colorf bg = {0.10f, 0.18f, 0.24f, 1.0f};

        if (nk_begin(nkContext, "Demo", nk_rect(50, 50, 230, 250),
                     NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                     NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(nkContext, 30, 80, 1);
            if (nk_button_label(nkContext, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(nkContext, 30, 2);
            if (nk_option_label(nkContext, "easy", op == EASY)) op = EASY;
            if (nk_option_label(nkContext, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(nkContext, 25, 1);
            nk_property_int(nkContext, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(nkContext, 20, 1);
            nk_label(nkContext, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(nkContext, 25, 1);
            if (nk_combo_begin_color(nkContext, nk_rgb_cf(bg), nk_vec2(nk_widget_width(nkContext),400))) {
                nk_layout_row_dynamic(nkContext, 120, 1);
                bg = nk_color_picker(nkContext, bg, NK_RGBA);
                nk_layout_row_dynamic(nkContext, 25, 1);
                bg.r = nk_propertyf(nkContext, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(nkContext, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(nkContext, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(nkContext, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(nkContext);
            }
        }
        nk_end(nkContext);

        calculator(nkContext);
        canvas(nkContext);
        overview(nkContext);
        node_editor(nkContext);

        file_browser_run(&browser, nkContext);

        return true;
    }
} // Tests