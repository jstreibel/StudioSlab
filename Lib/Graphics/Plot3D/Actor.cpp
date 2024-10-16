//
// Created by joao on 4/10/23.
//

#include "Actor.h"

#include <utility>

namespace Slab::Graphics {
    bool Actor::hasGUI() {
        return false;
    }

    void Actor::drawGUI() {
    }

    void Actor::setLabel(Str newLabel) {
        label = std::move(newLabel);
    }

    auto Actor::getLabel() const -> Str {
        return label;
    }

    void Actor::setVisibility(bool visibility) {
        visible = visibility;
    }

    bool Actor::isVisible() const {
        return visible;
    }
} // Graphics