//
// Created by joao on 11/8/24.
//

#ifndef STUDIOSLAB_SFMLSYSTEMWINDOW_H
#define STUDIOSLAB_SFMLSYSTEMWINDOW_H

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include "Graphics/Backend/PlatformWindow.h"
#include "SFMLEventTranslator.h"
#include "Utils/Threads.h"

namespace Slab::Graphics {

    class SFMLSystemWindow : public FPlatformWindow {
        friend class SFMLBackend;

        Vector<TVolatile<SFMLListener>> sfml_listeners;
        sf::RenderWindow *sfml_native_window = nullptr;
        sf::Font font;

    public:
        void Clear(const FColor&) const override;

    private:
        sf::Text text;

        Mutex off_sync;

        void PollEvents();

    protected:
        void Tick() override;
        void Flush() override;

    public:
        SFMLSystemWindow();

        bool addSFMLListener(const TVolatile<SFMLListener>& sfmlListener);

        Int GetHeight() const override;

        Int GetWidth() const override;

        bool ShouldClose() const override;

        void SignalClose() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_SFMLSYSTEMWINDOW_H
