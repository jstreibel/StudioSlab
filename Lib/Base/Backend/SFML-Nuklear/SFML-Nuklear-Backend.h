//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SFML_NUKLEAR_BACKEND_H
#define STUDIOSLAB_SFML_NUKLEAR_BACKEND_H

#include "../Backend.h"
#include "SFMLListener.h"
#include "Base/Controller/Nuklear/NuklearSFML.h"

#include <SFML/Graphics.hpp>

class SFMLNuklearBackend : public Backend {
    sf::RenderWindow *window;
    sf::Font font;
    sf::Text text;

    nk_context nkContext;

    bool running=true;

    std::vector<SFMLListener*> listeners;


public:
    SFMLNuklearBackend();

    void run(Program *program) override;

    void _treatEvents();

    void _render();
};


#endif //STUDIOSLAB_SFML_NUKLEAR_BACKEND_H
