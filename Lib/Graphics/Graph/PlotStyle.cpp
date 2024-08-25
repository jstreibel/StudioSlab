//
// Created by joao on 3/25/23.
//

#include "PlotStyle.h"
#include "Utils/Utils.h"

#include <memory>
#include <utility>
#include <map>

#define FILLED true
#define NOT_FILLED false



namespace Slab::Graphics {

    PlotStyle::PlotStyle(Color color, LinePrimitive primitive, bool filled,
                         Color fillColor, float thickness)
    : lineColor(color)
    , primitive(primitive)
    , filled(filled)
    , fillColor(fillColor)
    , thickness(thickness) {
        if (filled && fillColor == Nil) {
            this->fillColor = color;
            this->fillColor.a *= .25;
        }

        stippleFactor = 1;

        switch (primitive) {
            case VerticalLines:
            case Lines:
            case Point:
            case Solid:
                break;
            case Dotted:
                stipplePattern = 0x1111; //BinaryToUInt("o   o   o   o   ", 'o', ' ');
                stippleFactor = 2;
                break;
            case Dashed:
                stipplePattern = (GLshort) Common::BinaryToUInt(" o o o o o o o o", 'o', ' ');
                stippleFactor = 10;
                break;
            case DotDashed:
                stipplePattern = (GLshort) Common::BinaryToUInt("o  ooo  o  ooo  ", 'o', ' ');
                stippleFactor = 2;
                break;
        }
    }

    Pointer<PlotStyle> PlotStyle::clone() {
        return New<PlotStyle>(lineColor, primitive, filled, fillColor, thickness);
    }

    PlotStyle PlotStyle::permuteColors(bool odd) const {
        return PlotStyle(lineColor.permute(odd), primitive, filled, fillColor.permute(odd), thickness);
    }

}