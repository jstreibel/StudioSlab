//
// Created by joao on 11/4/21.
//

#include "SFML-Nuklear-Backend.h"
#include "COMPILE_CONFIG.h"

#include <Base/Controller/Nuklear/_nuklear_sfml.hpp>

SFMLNuklearBackend::SFMLNuklearBackend() : Backend("SFML+Nuklear backend") {

    window = new sf::RenderWindow(
            sf::VideoMode(1920, 1080),
            "SFML Window");

    nk_init_default(&nkContext, nullptr);
}

void SFMLNuklearBackend::run(Program *program) {

    sf::Clock timer; timer.restart();
    while(running){

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

        for(auto l : listeners)
            l->event(event);
    }
    nk_input_end(&nkContext);
}

void SFMLNuklearBackend::_render() {
    window->clear();

    for(auto l : listeners) {
        l->drawUI(&nkContext);
        l->render(window);
    }

    window->display();
}