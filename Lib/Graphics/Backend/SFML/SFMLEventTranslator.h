//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_SFMLEVENTTRANSLATOR_H
#define STUDIOSLAB_SFMLEVENTTRANSLATOR_H

#include "Graphics/Backend/Events/SystemWindowEventTranslator.h"
#include "SFMLListener.h"

namespace Slab::Graphics {

    class FSFMLSystemWindow;

    class FSFMLEventTranslator final : public FEventTranslator, public FSFMLListener {
        FSFMLSystemWindow* Owner;

    public:
        FSFMLEventTranslator() = delete;

        explicit FSFMLEventTranslator(FSFMLSystemWindow* owner);

        void event(const sf::Event &event) override;

        void render(sf::RenderWindow *window) override;


    };

    using SFMLEventTranslator [[deprecated("Use FSFMLEventTranslator")]] = FSFMLEventTranslator;

} // Core

#endif //STUDIOSLAB_SFMLEVENTTRANSLATOR_H
