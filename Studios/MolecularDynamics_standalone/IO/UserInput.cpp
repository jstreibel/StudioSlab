#include "UserInput.h"

#include <iostream>

#include "SFML/Window/Event.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"

#include "Hamiltonians/Types.h"

using namespace std;

UserInput *UserInput::myInstance = nullptr;

UserInput::UserInput(sf::RenderWindow *window) : mWindow(window), mFinished(false)
{

}

void UserInput::Initialize(sf::RenderWindow *window) {
    if(myInstance != nullptr) myInstance->mWindow = window;
    else UserInput::myInstance = new UserInput(window);
}

UserInput *UserInput::getInstance() {
    if(myInstance == nullptr){
        myInstance = new UserInput(nullptr);
    }

    return UserInput::myInstance;
}

void UserInput::treatEvents()
{
    sf::Event event{};
    while(mWindow->pollEvent(event)){
        if (event.type == sf::Event::Closed){
            mFinished = true;
        } else if (event.type == sf::Event::KeyPressed) {
            for(auto *keyPressListener : keyPressListeners)
                keyPressListener->KeyPressed(event.key, 1);
        } else if (event.type == sf::Event::KeyReleased) {
            for(auto *keyPressListener : keyPressListeners)
                keyPressListener->KeyPressed(event.key, 0);

        } else if (event.type == sf::Event::Resized) {
            const unsigned w = event.size.width, h = event.size.height;
            sf::FloatRect visibleArea(0, 0, w, h);
            mWindow->setView(sf::View(visibleArea));
        }
    }
}

bool UserInput::isFinished() const
{
    return this->mFinished;
}

void UserInput::addKeyPressListener(KeyPressListener *keyPressListener) {
    keyPressListeners.push_back(keyPressListener);
}