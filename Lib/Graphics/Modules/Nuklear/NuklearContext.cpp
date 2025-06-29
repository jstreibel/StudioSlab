//
// Created by joao on 11/18/24.
//

#include "NuklearContext.h"
#include "3rdParty/Nuklear/nuklear_glfw_gl4.h"

#include "Graphics/Backend/SystemWindow.h"
#include "Core/Tools/Resources.h"

namespace Slab::Graphics {

    constexpr Int Max_Vertex_Buffer  = 2*1024*1024;
    constexpr Int Max_Element_Buffer = 2*1024*1024;

    NuklearContext::NuklearContext(SystemWindow *window)
    : GUIContext(window) {
        // nkContext = nk_sfml_init(renderWindow);
        // /* Load Fonts: if none of these are loaded a default font will be used  */
        // /* Load Cursor: if you uncomment cursor loading please hide the cursor */
        // struct nk_font_atlas *atlas;
        // nk_sfml_font_stash_begin(&atlas);
        //
        // auto font_filename = Slab::Core::Resources::fontFileName(1);
        // struct nk_font *droid = nk_font_atlas_add_from_file(atlas, font_filename.c_str(), 22, nullptr);
        // /* struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0); */
        // /* struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0); */
        // /* struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0); */
        // /* struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0); */
        // /* struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0); */
        // nk_sfml_font_stash_end();
        //
        // /* nk_style_load_all_cursors(ctx, atlas->cursors);*/
        // if(droid == nullptr) Core::Log::Error() << "Could not load font '" << font_filename << "'. Using default Nuklear font." << Core::Log::Flush;
        // else nk_style_set_font(nkContext, &droid->handle);


        context = nk_glfw3_init((GLFWwindow*)window->GetRawPlatformWindowPointer(),
                                NK_GLFW3_DEFAULT, Max_Vertex_Buffer, Max_Element_Buffer);

        /* Load Fonts: if none of these are loaded a default font will be used  */
        /* Load Cursor: if you uncomment cursor loading please hide the cursor */
        {
            struct nk_font_atlas *atlas;
            nk_glfw3_font_stash_begin(&atlas);
            auto fontFolder = Core::Resources::FontsFolder;
            struct nk_font *droid =     nk_font_atlas_add_from_file(atlas, (fontFolder + "imgui/DroidSans.ttf").c_str(), 24, 0);
            // struct nk_font *roboto =    nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);
            // struct nk_font *future =    nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);
            // struct nk_font *clean =     nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);
            // struct nk_font *tiny =      nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);
            // struct nk_font *cousine =   nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);
            nk_glfw3_font_stash_end();
            /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
            nk_style_set_font(context, &droid->handle);
        }
    }

    void NuklearContext::NewFrame() {
        nk_input_begin(context);
    }

    void NuklearContext::AddMainMenuItem(MainMenuItem item) {

    }

    void NuklearContext::Bind() {
        
    }

    void NuklearContext::Render() const {

    }

    void *NuklearContext::GetContextPointer() {
        return context;
    }

} // Slab::Graphics