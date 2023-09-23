//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SFML_NUKLEAR_BACKEND_H
#define STUDIOSLAB_SFML_NUKLEAR_BACKEND_H

#include "../GUIBackend.h"
#include "SFMLListener.h"

#include <SFML/Graphics.hpp>

class SFMLNuklearBackend : public GUIBackend {
    nk_context nkContext;

    sf::RenderWindow *window;
    sf::Font font;
    sf::Text text;

    std::vector<SFMLListener*> sfmlListeners;

    bool running=true;
    bool paused=false;

    void _treatEvents();

    void _render();

public:
    SFMLNuklearBackend();

    void run(Program *program) override;

    auto getRenderWindow() -> sf::RenderWindow&;

    auto pause() -> void override;

    auto resume() -> void override;

    auto getScreenHeight() const -> Real override;

    static SFMLNuklearBackend& GetInstance();

    auto requestRender() -> void override;

    auto getMouseState() const -> const MouseState override;
};


#endif //STUDIOSLAB_SFML_NUKLEAR_BACKEND_H
