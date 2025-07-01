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

    NuklearSFMLModule::NuklearSFMLModule() {
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