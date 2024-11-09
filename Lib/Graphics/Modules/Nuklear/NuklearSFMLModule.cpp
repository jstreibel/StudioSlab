//
// Created by joao on 28/09/23.
//

#include "Core/Backend/BackendManager.h"
#include "Graphics/Backend/SFML/SFMLBackend.h"
#include "Core/Tools/Log.h"

#include "NuklearSFMLModule.h"
#define NK_SFML_GL3_IMPLEMENTATION
#include "3rdParty/Nuklear/nuklear_sfml_gl3.h"
#include "Core/Tools/Resources.h"
#include "Core/SlabCore.h"
#include "Graphics/Backend/SFML/SFMLSystemWindow.h"

#define MAX_VERTEX_BUFFER (1024 * 1024)
#define MAX_ELEMENT_BUFFER (1024 * 1024)

namespace Slab::Graphics {

    NuklearSFMLModule::NuklearSFMLModule()
            : NuklearModule() {
        try {
            auto sfmlBackend = DynamicPointerCast<SFMLBackend>(Core::GetBackend());
            // renderWindow = &sfmlBackend->GetMainSystemWindow();

            static auto myReference = Naked(*this);
            DynamicPointerCast<SFMLSystemWindow>
                    (sfmlBackend->GetMainSystemWindow())->addSFMLListener(myReference);

        } catch (std::bad_cast &e) {
            Core::Log::Error() << "Trying to instantiate Nuklear SFML module, but backend doesn't seem "
                            "to be SFML." << Core::Log::Flush;
            return;
        }

        nkContext = nk_sfml_init(renderWindow);

        /* Load Fonts: if none of these are loaded a default font will be used  */
        /* Load Cursor: if you uncomment cursor loading please hide the cursor */
        struct nk_font_atlas *atlas;
        nk_sfml_font_stash_begin(&atlas);

        auto font_filename = Slab::Core::Resources::fontFileName(1);
        struct nk_font *droid = nk_font_atlas_add_from_file(atlas, font_filename.c_str(), 22, nullptr);
        /* struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0); */
        /* struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0); */
        /* struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0); */
        /* struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0); */
        /* struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0); */
        nk_sfml_font_stash_end();

        /* nk_style_load_all_cursors(ctx, atlas->cursors);*/
        if(droid == nullptr) Core::Log::Error() << "Could not load font '" << font_filename << "'. Using default Nuklear font." << Core::Log::Flush;
        else nk_style_set_font(nkContext, &droid->handle);
    }

    NuklearSFMLModule::~NuklearSFMLModule() { nk_sfml_shutdown(); }

    void NuklearSFMLModule::event(const sf::Event &event) {
        nk_sfml_handle_event(&const_cast<sf::Event &>(event));
    }


    void NuklearSFMLModule::Update() {
        GraphicsModule::Update();

        /* IMPORTANT: `nk_sfml_render` modifies some global OpenGL state
        * with blending, scissor, face culling and depth test and defaults everything
        * back into a default state. Make sure to either save and restore or
        * reset your own state after drawing rendering the UI. */
        nk_sfml_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    }


}