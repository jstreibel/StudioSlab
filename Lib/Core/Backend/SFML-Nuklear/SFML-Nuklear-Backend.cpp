//
// Created by joao on 11/4/21.
//

#include "SFML-Nuklear-Backend.h"
#include "COMPILE_CONFIG.h"
#include "Core/Backend/BackendManager.h"

#include <Core/Controller/Nuklear/_nuklear_sfml.hpp>

SFMLNuklearBackend::SFMLNuklearBackend() : GUIBackend("SFML+Nuklear backend") {

    window = new sf::RenderWindow(
            sf::VideoMode(1920, 1080),
            "SFML Window");

    window->setFramerateLimit(0);
    window->setPosition(sf::Vector2i(250, 250));

    nk_init_default(&nkContext, nullptr);
}

void SFMLNuklearBackend::run(Program *program) {

    sf::Clock timer; timer.restart();
    while(running){

        if(!paused)
            program->cycle(Program::CycleOptions(1));

#if LIMIT_SIM_SPEED == false
        constexpr const auto frameInterval = _FRAME_INTERVAL_MSEC;
        auto elTimeMSec = (double)timer.getElapsedTime().asMilliseconds();
        if(elTimeMSec > frameInterval)
#endif
        {
            _treatEvents();
            _render();

            timer.restart();
        }

        running = window->isOpen();
    }
}

void SFMLNuklearBackend::_treatEvents() {

    sf::Event event{};
    bool exitEvent = false;

    nk_input_begin(&nkContext);
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            exitEvent = true;
        else if(event.type == sf::Event::KeyPressed) {
            sf::Keyboard::Key key = event.key.code;
            if(key == sf::Keyboard::Escape)
                exitEvent = true;
        }

        if(exitEvent){
            window->close();
            break;
        }

        nk_sfml_handle_event(event);

        for(auto l : sfmlListeners)
            l->event(event);
    }
    nk_input_end(&nkContext);
}

void SFMLNuklearBackend::_render() {
    window->clear();

    for(auto &l : sfmlListeners) {
        l->drawUI(&nkContext);
        l->render(window);
    }

    for(auto &listener : GetInstance().listeners )
        listener->notifyRender();

    window->display();
}

auto SFMLNuklearBackend::getRenderWindow() -> sf::RenderWindow & {
    return *window;
}

void SFMLNuklearBackend::pause() {
    paused = true;
}

void SFMLNuklearBackend::resume() {
    paused = false;
}

Real SFMLNuklearBackend::getScreenHeight() const {
    throw Str(__PRETTY_FUNCTION__) + " not implemented ";
}

SFMLNuklearBackend &SFMLNuklearBackend::GetInstance() {
    assert(Core::BackendManager::GetImplementation() == Core::SFML);

    auto &guiBackend = Core::BackendManager::GetGUIBackend();

    return *dynamic_cast<SFMLNuklearBackend*>(&guiBackend);
}

void SFMLNuklearBackend::requestRender() {
    throw Str(__PRETTY_FUNCTION__) + " not implemented";
}
