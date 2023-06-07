//
// Created by joao on 30/05/2021.
//

#include "Printing.h"

const auto dpi = 800;
const auto textWidthInches = 6.47699;

auto Printing::getDPI() -> int {
    return dpi;
}

auto Printing::getTextWidthInches() -> Real {
    return textWidthInches;
}

auto Printing::getTotalHorizontalDots(Real proportionOfTextWidth) -> int {
    return dpi*(textWidthInches*proportionOfTextWidth);
}

auto Printing::getTotalVerticalDots(Real proportionOfTextWidth) -> int {
    return dpi*(textWidthInches*proportionOfTextWidth);
}


