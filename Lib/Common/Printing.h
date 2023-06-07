//
// Created by joao on 30/05/2021.
//

#ifndef V_SHAPE_PRINTING_H
#define V_SHAPE_PRINTING_H

#include "Types.h"

namespace Printing {
    auto getDPI() -> int;
    auto getTextWidthInches() -> Real;

    auto getTotalHorizontalDots(Real proportionOfTextWidth) -> int;
    auto getTotalVerticalDots(Real proportionOfTextWidth) -> int;
}


#endif //V_SHAPE_PRINTING_H
