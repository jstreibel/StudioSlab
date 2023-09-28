//
// Created by joao on 27/09/23.
//

#include "SFMLEventTranslator.h"

namespace Core {
    void SFMLEventTranslator::event(const sf::Event &event) {
        SFMLListener::event(event);
    }

    void SFMLEventTranslator::render(sf::RenderWindow *window) {
        SFMLListener::render(window);
    }

    void SFMLEventTranslator::drawUI(nk_context *pContext) {
        SFMLListener::drawUI(pContext);
    }
} // Core