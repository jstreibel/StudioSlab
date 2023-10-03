//
// Created by joao on 3/25/23.
//

#include "Styles.h"
#include "Utils/Utils.h"

#include <memory>
#include <utility>
#include <map>

#define FILLED true
#define NOT_FILLED false



Styles::PlotStyle::PlotStyle(Color color, Styles::Primitive primitive, bool filled,
                             Color fillColor, float thickness)
        : lineColor(color)
        , primitive(primitive)
        , filled(filled)
        , fillColor(fillColor)
        , thickness(thickness)
        , visible(true) {
    if(filled && fillColor==Nil) {
        this->fillColor = color;
        this->fillColor.a *= .25;
    }

    stippleFactor = 1;

    switch (primitive) {
        case VerticalLines:
        case Lines:
        case Point:
        case SolidLine:
            break;
        case DottedLine:
            stipplePattern = 0x1111; //BinaryToUInt("o   o   o   o   ", 'o', ' ');
            stippleFactor = 2;
            break;
        case DashedLine:
            stipplePattern = (GLshort)Common::BinaryToUInt(" o o o o o o o o", 'o', ' ');
            stippleFactor = 10;
            break;
        case DotDashedLine:
            stipplePattern = (GLshort)Common::BinaryToUInt("o  ooo  o  ooo  ", 'o', ' ');
            stippleFactor = 2;
            break;
    }
}

Styles::PlotStyle Styles::PlotStyle::permuteColors(bool odd) const {
    return Styles::PlotStyle(lineColor.permute(odd), primitive, filled, fillColor.permute(odd), thickness);
}


Styles::ColorScheme_ptr Styles::NewScheme(const Styles::StyleScheme& scheme) {
    { return std::make_shared<Styles::StyleScheme>(scheme); }
}

Styles::StyleScheme::StyleScheme(Color graphBackground,
                                 Color graphNumbersColor,
                                 Color graphTitleColor,
                                 Color axisColor,
                                 Color majorTickColor,
                                 PlotStyle XHairStyle,
                                 PlotStyle gridStyle,
                                 std::shared_ptr<Graphics::Writer> labelsWriter,
                                 std::shared_ptr<Graphics::Writer> ticksWriter,
                                 std::vector<PlotStyle> plotStyles)
                                 : graphBackground(graphBackground)
                                 , graphNumbersColor(graphNumbersColor)
                                 , graphTitleColor(graphTitleColor)
                                 , axisColor(axisColor)
                                 , majorTickColor(majorTickColor)
                                 , XHairStyle(XHairStyle)
                                 , gridLines(gridStyle)
                                 , labelsWriter(std::move(labelsWriter))
                                 , ticksWriter(std::move(std::move(ticksWriter)))
                                 , funcPlotStyles(std::move(plotStyles))
                                 , vTickHeightinPixels(5)
                                 , vAxisPaddingInPixels(60)
{

}

