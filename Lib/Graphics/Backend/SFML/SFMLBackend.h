//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SFMLBACKEND_H
#define STUDIOSLAB_SFMLBACKEND_H

#include "Graphics/Backend/GraphicBackend.h"
#include "SFMLListener.h"
#include "SFMLEventTranslator.h"

#include <SFML/Graphics.hpp>

namespace Slab::Graphics {

    class SFMLBackend : public GraphicBackend {
        Pointer<SFMLEventTranslator> sfmlEventTranslator=nullptr;
        Vector<Volatile<SFMLListener>> sfmlListeners;

        sf::RenderWindow *window;
        sf::Font font;
        sf::Text text;

        Mutex off_sync;
        bool running = true;

        void _treatEvents();

        void _render();

    public:
        SFMLBackend();

        sf::RenderWindow &getMainWindow();

        bool addSFMLListener(const Volatile<SFMLListener>& sfmlListener);

        void run() override;

        void terminate() override;

        auto getRenderWindow() -> sf::RenderWindow &;

        // auto getScreenHeight() const -> Real override;

        static SFMLBackend &GetInstance();

        auto getMouseState() const -> MouseState override;
    };

}

#endif //STUDIOSLAB_SFMLBACKEND_H
