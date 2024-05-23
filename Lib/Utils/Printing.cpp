//
// Created by joao on 30/05/2021.
//

#include "Printing.h"

const auto dpi = 800;
const auto LaTEXTextWidthInches = 6.47699;

auto Slab::Printing::getDPI() -> int {
    return dpi;
}

auto Slab::Printing::getTextWidthInches() -> Real {
    return LaTEXTextWidthInches;
}

auto Slab::Printing::getTotalHorizontalDots(Real proportionOfTextWidth) -> int {
    return dpi*(LaTEXTextWidthInches*proportionOfTextWidth);
}

auto Slab::Printing::getTotalVerticalDots(Real proportionOfTextWidth) -> int {
    return dpi*(LaTEXTextWidthInches*proportionOfTextWidth);
}


