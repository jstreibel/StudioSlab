//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SFML_NUKLEAR_BACKEND_H
#define STUDIOSLAB_SFML_NUKLEAR_BACKEND_H

#include "../GUIBackend.h"
#include "SFMLListener.h"
#include "Core/Controller/Nuklear/NuklearSFML.h"

#include <SFML/Graphics.hpp>

class SFMLNuklearBackend : public GUIBackend {
    sf::RenderWindow *window;
    sf::Font font;
    sf::Text text;

    nk_context nkContext;


    std::vector<SFMLListener*> listeners;

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


};


#endif //STUDIOSLAB_SFML_NUKLEAR_BACKEND_H
