//
// Created by joao on 12/04/2020.
//

#ifndef MOLEKUL_PLAY_KEYPRESSLISTENER_H
#define MOLEKUL_PLAY_KEYPRESSLISTENER_H


#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

class KeyPressListener {
public:
    KeyPressListener();


    virtual void KeyPressed(sf::Event::KeyEvent &keyEvent, int value) = 0;
};


#endif //MOLEKUL_PLAY_KEYPRESSLISTENER_H
