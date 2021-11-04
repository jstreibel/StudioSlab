//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SFMLLISTENER_H
#define STUDIOSLAB_SFMLLISTENER_H

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <Studios/Controller/Nuklear/NuklearSFML.h>

class SFMLListener {
public:
    virtual void event(const sf::Event &event)    { };
    virtual void render(sf::RenderWindow *window) { };
    virtual void drawUI(nk_context *pContext)     { };
};


#endif //STUDIOSLAB_SFMLLISTENER_H
