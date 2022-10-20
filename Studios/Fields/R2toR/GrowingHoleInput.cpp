//
// Created by joao on 10/11/22.
//

#include "GrowingHoleInput.h"

#include "Common/Utils.h"

#include "GrowingHoleBoundaryCondition.h"

#include "OutputBuilder.h"

const auto pi = 3.1415926535897932384626;


R2toR::InputGrowingHole::InputGrowingHole() : R2toRBCInterface("(2+1)-d Shockwave as a growing hole.", "gh",
                                                               new OutputBuilder)
{
    addParameters({&height});
}

const void *R2toR::InputGrowingHole::getBoundary() const {
    const double a = *this->height; // Eh isso mesmo? Não era h = 2a/3??

    return new GrowingHoleBoundaryCondition(a);
}