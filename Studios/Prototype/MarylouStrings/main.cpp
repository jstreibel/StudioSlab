//
// Created by joao on 25/06/2021.
//


#include "Base/Graphics/SFML/Graph.h"
#include "Base/Controller/Nuklear/NuklearSFML.h"

#include "Integrator.h"

const uint32_t MaxVertexMemory = 512 * 1024;
const uint32_t MaxElementMemory = 128 * 1024;

nk_context *ctx = nullptr;
sf::RenderWindow window{{2000, 900}, "Nuklear SFML"};

void InitGUI() {
    ctx = nk_sfml_init(&window);

    nk_font_atlas *atlas{nullptr};
    nk_sfml_font_stash_begin(&atlas);
    // Adding fonts:
    // nk_font *font = nk_font_atlas_add_from_file(atlas, "path", height, config);
    nk_sfml_font_stash_end();

    // Setting a font:
    // nk_style_set_font(ctx, &font->handle);
}

int main(int, const char **)
{
    InitGUI();
    MaryApp app(window);

    bool running = true;
    sf::Event event{};

    while (running) {
        nk_input_begin(ctx);
        while (window.pollEvent(event)) {
            nk_sfml_handle_event(event);

            if (event.type == sf::Event::Closed) {
                running = false;
                break;
            }
        }
        nk_input_end(ctx);

        app.setupGUI(ctx);

        window.clear();
        {
            app.draw(window);

            nk_sfml_render(NK_ANTI_ALIASING_ON, MaxVertexMemory, MaxElementMemory);
        }
        window.display();
    }

    window.close();
    nk_sfml_shutdown();

    return 0;
}