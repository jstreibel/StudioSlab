//
// Created by joao on 11/4/21.
//

#include "SFMLBackend.h"
#include "COMPILE_CONFIG.h"
#include "Core/Backend/BackendManager.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Utils/ReferenceIterator.h"


namespace Slab::Core {


    SFMLBackend::SFMLBackend() : GraphicBackend("SFML backend", sfmlEventTranslator) {

        sf::ContextSettings contextSettings;
        contextSettings.depthBits = 24;
        contextSettings.majorVersion = 4;
        contextSettings.minorVersion = 6;
        contextSettings.antialiasingLevel = 8;

        window = new sf::RenderWindow(
                sf::VideoMode(1920, 1080),
                "SFML Window", sf::Style::Default, contextSettings);

        window->setFramerateLimit(0);
        window->setPosition(sf::Vector2i(250, 250));

        addSFMLListener(sfmlEventTranslator);
    }

    void SFMLBackend::run() {

        sf::Clock timer;
        timer.restart();
        while (running) {
#if LIMIT_SIM_SPEED == false
            constexpr const auto frameInterval = _FRAME_INTERVAL_MSEC;
            auto elTimeMSec = (double) timer.getElapsedTime().asMilliseconds();
            if (elTimeMSec > frameInterval)
#endif
            {
                _treatEvents();
                _render();

                timer.restart();
            }

            running = window->isOpen();
        }
    }

    void SFMLBackend::_treatEvents() {

        sf::Event event{};
        bool exitEvent = false;

        IterateReferences(graphicModules, FuncRun(beginEvents));

        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                exitEvent = true;
            else if (event.type == sf::Event::KeyPressed) {
                sf::Keyboard::Key key = event.key.code;
                if (key == sf::Keyboard::Escape)
                    exitEvent = true;
            }

            if (exitEvent) {
                window->close();
                break;
            }

            IterateReferences(sfmlListeners, FuncRun(event, event));
        }

        IterateReferences(graphicModules, FuncRun(endEvents));
    }

    void SFMLBackend::_render() {
        window->clear();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        IterateReferences(graphicModules, FuncRun(beginRender));

        auto win = window;
        IterateReferences(sfmlListeners, FuncRun(render, win));

        IterateReferences(graphicModules, FuncRun(endRender));

        window->display();
    }

    auto SFMLBackend::getRenderWindow() -> sf::RenderWindow & { return *window; }

    Real SFMLBackend::getScreenHeight() const { throw Str(__PRETTY_FUNCTION__) + " not implemented "; }

    SFMLBackend &SFMLBackend::GetInstance() {
        // assert(Core::BackendManager::GetImplementation() == Core::SFML);

        auto &guiBackend = Core::BackendManager::GetGUIBackend();

        return *dynamic_cast<SFMLBackend *>(&guiBackend);
    }

    MouseState SFMLBackend::getMouseState() const {
        NOT_IMPLEMENTED_CLASS_METHOD

        return MouseState();
    }

    bool SFMLBackend::addSFMLListener(const Reference<SFMLListener>& sfmlListener) {
        if (ContainsReference(sfmlListeners, sfmlListener)) return false;

        sfmlListeners.emplace_back(sfmlListener);

        return true;
    }

    sf::RenderWindow &SFMLBackend::getMainWindow() {
        return *window;
    }

    void SFMLBackend::terminate() {
        window->close();
    }

}