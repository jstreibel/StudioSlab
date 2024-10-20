//
// Created by joao on 11/4/21.
//

#include "SFMLBackend.h"
#include "Graphics/SlabGraphics.h"

#include "COMPILE_CONFIG.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Utils/ReferenceIterator.h"


namespace Slab::Graphics {


    SFMLBackend::SFMLBackend() : GraphicBackend("SFML backend", (sfmlEventTranslator=New<SFMLEventTranslator>())) {

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

        auto eventTranslator = getEventTranslator();
        // sfmlEventTranslator = DynamicPointerCast<Core::SFMLEventTranslator>(eventTranslator);
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
            off_sync.lock();

            _treatEvents();
            _render();

            timer.restart();

            running = window->isOpen();

            off_sync.unlock();
        }
    }

    void SFMLBackend::_treatEvents() {

        sf::Event event{};

        IterateReferences(graphicModules, FuncRun(beginEvents));

        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                terminate();
                break;
            }
            else if (event.type == sf::Event::KeyPressed) {
                sf::Keyboard::Key key = event.key.code;
                // if (key == sf::Keyboard::Escape);
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

    // Real SFMLBackend::getScreenHeight() const { NOT_IMPLEMENTED_CLASS_METHOD }

    SFMLBackend &SFMLBackend::GetInstance() {
        // assert(Core::BackendManager::GetImplementation() == Core::SFML);

        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        return *dynamic_cast<SFMLBackend *>(&guiBackend);
    }

    MouseState SFMLBackend::getMouseState() const {
        NOT_IMPLEMENTED_CLASS_METHOD

        // return MouseState();
    }

    bool SFMLBackend::addSFMLListener(const Volatile<SFMLListener>& sfmlListener) {
        if (ContainsReference(sfmlListeners, sfmlListener)) return false;

        sfmlListeners.emplace_back(sfmlListener);

        return true;
    }

    sf::RenderWindow &SFMLBackend::getMainWindow() {
        return *window;
    }

    void SFMLBackend::terminate() {
        off_sync.lock();

        window->close();
        clearModules();
        clearListeners();
        sfmlEventTranslator->clear();

        off_sync.unlock();
    }

}