//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SFMLLISTENER_H
#define STUDIOSLAB_SFMLLISTENER_H

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class FSFMLListener {
public:
    virtual void event(const sf::Event &event)    { };
    virtual void render(sf::RenderWindow *window) { };
};

using SFMLListener [[deprecated("Use FSFMLListener")]] = FSFMLListener;


#endif //STUDIOSLAB_SFMLLISTENER_H
