//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SFML_NUKLEAR_BACKEND_H
#define STUDIOSLAB_SFML_NUKLEAR_BACKEND_H

#include "../Backend.h"
#include "SFMLListener.h"

#include <SFML/Graphics.hpp>
#include <Base/Controller/Nuklear/NuklearSFML.h>

class SFMLNuklearBackend : public Backend {
    sf::RenderWindow *window;
    sf::Font font;
    sf::Text text;

    nk_context *nkContext = nullptr;

    bool running=true;

    SFMLNuklearBackend();

    std::vector<SFMLListener*> listeners;

public:
    static SFMLNuklearBackend *GetInstance();

    void run(Program *integrator) override;

    void _runPrograms();

    void _treatEvents();

    void _render();
};


#endif //STUDIOSLAB_SFML_NUKLEAR_BACKEND_H
