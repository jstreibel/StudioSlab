//
// Created by joao on 11/4/21.
//

#include "Space.h"

Space::Space(PosInt dim) : dim(dim) {   }

Space::Space(const Space &space) : Space(space.dim)
{   }

auto Space::getDim() const -> PosInt {
    return dim;
}
