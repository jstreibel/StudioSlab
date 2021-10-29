//
// Created by joao on 30/05/2021.
//

#include "Printing.h"

const auto dpi = 200;
const auto textWidthInches = 6.47699;

auto Printing::getDPI() -> int {
    return dpi;
}

auto Printing::getTextWidthInches() -> double {
    return textWidthInches;
}

auto Printing::getTotalHorizontalDots(double proportionOfTextWidth) -> int {
    return dpi*(textWidthInches*proportionOfTextWidth);
}

auto Printing::getTotalVerticalDots(double proportionOfTextWidth) -> int {
    return dpi*(textWidthInches*proportionOfTextWidth);
}


