//
// Created by joao on 10/20/24.
//

#include "Decorator.h"

namespace Slab::Graphics {
    void Decorator::operator()(const SlabWindow &slab_window) {
        hPadding;
        vPadding;

        auto rect = slab_window.getTotalWindowRect();

        fix x_min = rect.xMin;


    }
} // Slab::Graphics