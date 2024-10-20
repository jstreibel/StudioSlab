//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_SFMLEVENTTRANSLATOR_H
#define STUDIOSLAB_SFMLEVENTTRANSLATOR_H

#include "Graphics/Backend/Events/SystemWindowEventTranslator.h"
#include "SFMLListener.h"

namespace Slab::Graphics {

    class SFMLEventTranslator : public EventTranslator, public SFMLListener {
    public:
        void event(const sf::Event &event) override;

        void render(sf::RenderWindow *window) override;
    };

} // Core

#endif //STUDIOSLAB_SFMLEVENTTRANSLATOR_H
