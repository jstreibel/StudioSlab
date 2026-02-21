//
// Created by joao on 4/10/23.
//

#include "Actor.h"

#include <utility>

namespace Slab::Graphics {
    bool FActor::hasGUI() {
        return false;
    }

    void FActor::drawGUI() {
    }

    void FActor::setLabel(Str newLabel) {
        label = std::move(newLabel);
    }

    auto FActor::getLabel() const -> Str {
        return label;
    }

    void FActor::setVisibility(bool visibility) {
        visible = visibility;
    }

    bool FActor::isVisible() const {
        return visible;
    }
} // Graphics