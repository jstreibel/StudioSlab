//
// Created by joao on 30/05/2021.
//

#ifndef V_SHAPE_PRINTING_H
#define V_SHAPE_PRINTING_H


namespace Printing {
    auto getDPI() -> int;
    auto getTextWidthInches() -> double;

    auto getTotalHorizontalDots(double proportionOfTextWidth) -> int;
    auto getTotalVerticalDots(double proportionOfTextWidth) -> int;
}


#endif //V_SHAPE_PRINTING_H
