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

    class FSFMLSystemWindow : public FPlatformWindow {
        friend class FSFMLBackend;

        Vector<TVolatile<FSFMLListener>> sfml_listeners;
        sf::RenderWindow *sfml_native_window = nullptr;
        sf::Font font;

    public:
        void Clear(const FColor&) const override;
        TPointer<IDrawBackend2D> GetRenderer() const override;

    private:
        sf::Text text;

        Mutex off_sync;

        void PollEvents();

    protected:
        void Tick() override;
        void Flush() override;

    public:
        FSFMLSystemWindow();

        bool AddSFMLListener(const TVolatile<FSFMLListener>& sfmlListener);
        [[deprecated("Use AddSFMLListener()")]] bool addSFMLListener(const TVolatile<FSFMLListener>& sfmlListener) { return AddSFMLListener(sfmlListener); }

        Int GetHeight() const override;

        Int GetWidth() const override;

        bool ShouldClose() const override;

        void SignalClose() override;
    };

    using SFMLSystemWindow [[deprecated("Use FSFMLSystemWindow")]] = FSFMLSystemWindow;

} // Slab::Graphics

#endif //STUDIOSLAB_SFMLSYSTEMWINDOW_H
