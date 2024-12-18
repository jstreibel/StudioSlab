//
// Created by joao on 11/8/24.
//

#include "SFMLSystemWindow.h"
#include "Utils/ReferenceIterator.h"

namespace Slab::Graphics {
    sf::RenderWindow *new_sfml_native_window(){
        sf::ContextSettings contextSettings;
        contextSettings.depthBits = 32;
        contextSettings.majorVersion = 4;
        contextSettings.minorVersion = 6;
        contextSettings.antialiasingLevel = 8;
        contextSettings.sRgbCapable = true;

        auto window = new sf::RenderWindow(
                sf::VideoMode(1920, 1080),
                "SFML Window", sf::Style::Default, contextSettings);

        window->setFramerateLimit(0);
        window->setPosition(sf::Vector2i(250, 250));

        return window;
    }

    SFMLSystemWindow::SFMLSystemWindow()
    : SystemWindow(new_sfml_native_window(), New<SFMLEventTranslator>())
    , sfml_native_window((sf::RenderWindow*)window_ptr)
    {
        addSFMLListener(DynamicPointerCast<SFMLEventTranslator>(event_translator));
    }

    Int SFMLSystemWindow::getWidth() const {
        return (Int)sfml_native_window->getSize().x;
    }

    Int SFMLSystemWindow::getHeight() const {
        return (Int)sfml_native_window->getSize().y;
    }

    void SFMLSystemWindow::Cycle() {
        sfml_native_window->clear();

        auto win = sfml_native_window;
        IterateReferences(sfml_listeners, FuncRun(render, win));

        sfml_native_window->display();
    }

    void SFMLSystemWindow::PollEvents() {
        sf::Event event{};

        off_sync.lock();
        while (sfml_native_window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                sfml_native_window->close();
                clearListeners();
                break;
            }
            else if (event.type == sf::Event::KeyPressed) {
                sf::Keyboard::Key key = event.key.code;
                // if (key == sf::Keyboard::Escape);
            }

            IterateReferences(sfml_listeners, FuncRun(event, event));
        }
        off_sync.unlock();
    }

    bool SFMLSystemWindow::addSFMLListener(const Volatile<SFMLListener> &sfmlListener) {
        if (ContainsReference(sfml_listeners, sfmlListener)) return false;

        sfml_listeners.emplace_back(sfmlListener);

        return true;
    }

    void SFMLSystemWindow::SignalClose() {
        off_sync.lock();

        sfml_native_window->close();
        clearListeners();

        off_sync.unlock();
    }

    bool SFMLSystemWindow::ShouldClose() const {
        return !sfml_native_window->isOpen();;
    }


} // Slab::Graphics