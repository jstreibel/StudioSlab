//
// Created by joao on 11/4/21.
//

#include "Space.h"


namespace Slab::Math {

    Space::Space(UInt dim) : dim(dim) {}

    Space::Space(const Space &space) : Space(space.dim) {}

    auto Space::getDim() const -> UInt {
        return dim;
    }
}