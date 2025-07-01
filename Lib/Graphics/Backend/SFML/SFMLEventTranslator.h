//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_SFMLEVENTTRANSLATOR_H
#define STUDIOSLAB_SFMLEVENTTRANSLATOR_H

#include "Graphics/Backend/Events/SystemWindowEventTranslator.h"
#include "SFMLListener.h"

namespace Slab::Graphics {

    class SFMLSystemWindow;

    class SFMLEventTranslator final : public FEventTranslator, public SFMLListener {
        SFMLSystemWindow *Owner;

    public:
        SFMLEventTranslator() = delete;

        explicit SFMLEventTranslator(SFMLSystemWindow* owner);

        void event(const sf::Event &event) override;

        void render(sf::RenderWindow *window) override;


    };

} // Core

#endif //STUDIOSLAB_SFMLEVENTTRANSLATOR_H
