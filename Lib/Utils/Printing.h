//
// Created by joao on 30/05/2021.
//

#ifndef V_SHAPE_PRINTING_H
#define V_SHAPE_PRINTING_H

#include "Types.h"

namespace Slab::Printing {
    auto GetDPI() -> int;
    [[deprecated("Use GetDPI")]]
    inline auto getDPI() -> int {
        return GetDPI();
    }

    auto GetTextWidthInches() -> DevFloat;
    [[deprecated("Use GetTextWidthInches")]]
    inline auto getTextWidthInches() -> DevFloat {
        return GetTextWidthInches();
    }

    auto GetTotalHorizontalDots(DevFloat proportionOfTextWidth) -> int;
    [[deprecated("Use GetTotalHorizontalDots")]]
    inline auto getTotalHorizontalDots(const DevFloat proportionOfTextWidth) -> int {
        return GetTotalHorizontalDots(proportionOfTextWidth);
    }

    auto GetTotalVerticalDots(DevFloat proportionOfTextWidth) -> int;
    [[deprecated("Use GetTotalVerticalDots")]]
    inline auto getTotalVerticalDots(const DevFloat proportionOfTextWidth) -> int {
        return GetTotalVerticalDots(proportionOfTextWidth);
    }
}


#endif //V_SHAPE_PRINTING_H
