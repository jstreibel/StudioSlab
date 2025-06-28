//
// Created by joao on 30/05/2021.
//

#ifndef V_SHAPE_PRINTING_H
#define V_SHAPE_PRINTING_H

#include "Types.h"

namespace Slab::Printing {
    auto getDPI() -> int;
    auto getTextWidthInches() -> DevFloat;

    auto getTotalHorizontalDots(DevFloat proportionOfTextWidth) -> int;
    auto getTotalVerticalDots(DevFloat proportionOfTextWidth) -> int;
}


#endif //V_SHAPE_PRINTING_H
