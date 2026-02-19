//
// Created by joao on 30/05/2021.
//

#include "Printing.h"

const auto dpi = 800;
const auto LaTEXTextWidthInches = 6.47699;

auto Slab::Printing::GetDPI() -> int {
    return dpi;
}

auto Slab::Printing::GetTextWidthInches() -> DevFloat {
    return LaTEXTextWidthInches;
}

auto Slab::Printing::GetTotalHorizontalDots(DevFloat proportionOfTextWidth) -> int {
    return dpi*(LaTEXTextWidthInches*proportionOfTextWidth);
}

auto Slab::Printing::GetTotalVerticalDots(DevFloat proportionOfTextWidth) -> int {
    return dpi*(LaTEXTextWidthInches*proportionOfTextWidth);
}

