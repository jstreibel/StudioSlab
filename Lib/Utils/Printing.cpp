//
// Created by joao on 30/05/2021.
//

#include "Printing.h"

const auto dpi = 800;
const auto LaTEXTextWidthInches = 6.47699;

auto Slab::Printing::getDPI() -> int {
    return dpi;
}

auto Slab::Printing::getTextWidthInches() -> DevFloat {
    return LaTEXTextWidthInches;
}

auto Slab::Printing::getTotalHorizontalDots(DevFloat proportionOfTextWidth) -> int {
    return dpi*(LaTEXTextWidthInches*proportionOfTextWidth);
}

auto Slab::Printing::getTotalVerticalDots(DevFloat proportionOfTextWidth) -> int {
    return dpi*(LaTEXTextWidthInches*proportionOfTextWidth);
}


