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
    , filled(filled)
    , fillColor(fillColor)
    , thickness(thickness) {
        if (filled && fillColor == Nil) {
            this->fillColor = color;
            this->fillColor.a *= .25;
        }

        stippleFactor = 1;

        setPrimitive(primitive);
    }

    Pointer<PlotStyle> PlotStyle::clone() {
        return New<PlotStyle>(lineColor, primitive, filled, fillColor, thickness);
    }

    PlotStyle PlotStyle::permuteColors(bool odd) const {
        return PlotStyle(lineColor.permute(odd), primitive, filled, fillColor.permute(odd), thickness);
    }

    auto PlotStyle::getPrimitive() const -> LinePrimitive {
        return primitive;
    }

    void PlotStyle::setPrimitive(LinePrimitive primmy) {
        primitive = primmy;

        switch (primitive) {
            case VerticalLines:
            case Lines:
            case Point:
            case Solid:
                stipplePattern = 0xffff;
                stippleFactor = 1;
                break;
            case Dotted:
                stipplePattern = 0x1111; //BinaryToUInt("o   o   o   o   ", 'o', ' ');
                stippleFactor = 2;
                break;
            case Dashed:
                stipplePattern = (GLshort) Common::BinaryToUInt(" ooo ooo ooo ooo", ' ', 'o');
                stippleFactor = 1;
                break;
            case DotDashed:
                stipplePattern = (GLshort) Common::BinaryToUInt("ooo o ooo o ooo ", ' ', 'o');
                stippleFactor = 2;
                break;
            case __COUNT__:
                throw Exception("__COUNT__ is not a valid primitive type @ " + Str(__PRETTY_FUNCTION__));
        }
    }

    auto PlotStyle::getStippleFactor() -> GLint {
        return stippleFactor;
    }

    auto PlotStyle::getStipplePattern() -> GLshort {
        return stipplePattern;
    }

    void PlotStyle::setStippleFactor(GLint factor) {
        stippleFactor = factor;
    }

    void PlotStyle::setStipplePattern(GLshort pattern) {
        stipplePattern = pattern;
    }

}