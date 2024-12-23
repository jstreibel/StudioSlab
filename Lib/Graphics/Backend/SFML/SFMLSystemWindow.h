//
// Created by joao on 11/8/24.
//

#ifndef STUDIOSLAB_SFMLSYSTEMWINDOW_H
#define STUDIOSLAB_SFMLSYSTEMWINDOW_H

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include "Graphics/Backend/SystemWindow.h"
#include "SFMLEventTranslator.h"
#include "Utils/Threads.h"

namespace Slab::Graphics {

    class SFMLSystemWindow : public SystemWindow {
        friend class SFMLBackend;

        Vector<Volatile<SFMLListener>> sfml_listeners;
        sf::RenderWindow *sfml_native_window = nullptr;
        sf::Font font;
        sf::Text text;

        Mutex off_sync;

        void PollEvents();

    protected:
        void Cycle() override;

    public:
        SFMLSystemWindow();

        bool addSFMLListener(const Volatile<SFMLListener>& sfmlListener);

        Int getHeight() const override;

        Int getWidth() const override;

        bool ShouldClose() const override;

        void SignalClose() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_SFMLSYSTEMWINDOW_H
