//
// Created by joao on 3/25/23.
//

#include "Styles.h"
#include "Utils/Utils.h"
#include "Utils/Resources.h"
#include <memory>
#include <utility>

#define FILLED true
#define NOT_FILLED false

// Rainbow
// Red: #FF0000
// Orange: #FF7F00
// Yellow: #FFFF00
// Green: #00FF00
// Blue: #0000FF
// Indigo: #4B0082
// Violet: #9400D3

Styles::ColorScheme_ptr current;

void SetSchemeDark();
void SetSchemeDark2();
void SetSchemeHacker();
void SetSchemePrint();
void SetSchemeLight();
void SetSchemeElegant();

void Styles::Init() {
    if(current != nullptr) return;
    SetSchemeDark();
    // SetSchemeDark2();
    // SetSchemeHacker();
    // SetSchemePrint();
    // SetSchemeLight();
    // SetSchemeElegant();

}


void SetSchemeDark () {
    Styles::Color graphTitleColor   = {1   ,1   ,1   ,1};
    Styles::Color graphNumbersColor = {1   ,1   ,1   , .8};
    Styles::Color background        = {0.05,0.05,0.08,1};

    Styles::Color axisColor =       {1,1,1,.85};
    Styles::Color tickColor =       {1,1,1,.75};

    // https://www.learnui.design/tools/data-color-picker.html
    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{Styles::Color::FromHex("#004c6d"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#1d617f"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#347691"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#498ca3"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#5fa2b5"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#76b9c7"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#8ed0da"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#a7e7ec"), Styles::Primitive::SolidLine}
    };

    auto XHairColor = axisColor;
    XHairColor.a = .50;
    auto XHairStyle = Styles::PlotStyle(axisColor);
    XHairStyle.primitive = Styles::Lines;
    XHairStyle.thickness = 1;

    auto gridLinesScheme = Styles::PlotStyle(tickColor, Styles::DotDashedLine, false, Styles::Nil, 0.8);
    gridLinesScheme.lineColor.a = 0.15;

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 22);
    current = Styles::NewScheme({background, graphNumbersColor, graphTitleColor, axisColor, tickColor, XHairStyle, gridLinesScheme, &writer, &writer, graphs});

}

void SetSchemeDark2 () {
    Styles::Color graphTitleFont =  {1,1,1,1};
    Styles::Color graphTicksFont =  {1,1,1,.8};
    Styles::Color background =      {0.05,0.05,0.08,1};

    Styles::Color axisColor =       {1,1,1,.25};
    Styles::Color tickColor =       {1,1,1,.25};

    // https://www.learnui.design/tools/data-color-picker.html
    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{Styles::Color::FromHex("#003f5c"), Styles::Primitive::SolidLine, true, Styles::Nil, 1.5},
            Styles::PlotStyle{Styles::Color::FromHex("#2f4b7c"), Styles::Primitive::SolidLine, true, Styles::Nil, 1.5},
            Styles::PlotStyle{Styles::Color::FromHex("#665191"), Styles::Primitive::SolidLine, true, Styles::Nil, 1.5},
            Styles::PlotStyle{Styles::Color::FromHex("#a05195"), Styles::Primitive::SolidLine, true, Styles::Nil, 1.5},
            Styles::PlotStyle{Styles::Color::FromHex("#d45087"), Styles::Primitive::SolidLine, true, Styles::Nil, 1.5},
            Styles::PlotStyle{Styles::Color::FromHex("#f95d6a"), Styles::Primitive::SolidLine, true, Styles::Nil, 1.5},
            Styles::PlotStyle{Styles::Color::FromHex("#ff7c43"), Styles::Primitive::SolidLine, true, Styles::Nil, 1.5},
            Styles::PlotStyle{Styles::Color::FromHex("#ffa600"), Styles::Primitive::SolidLine, true, Styles::Nil, 1.5}
    };

    auto XHairColor = axisColor;
    XHairColor.a = .89;
    auto XHairStyle = Styles::PlotStyle(axisColor);
    XHairStyle.primitive = Styles::DashedLine;
    XHairStyle.thickness = 1;

    auto gridLinesScheme = Styles::PlotStyle(tickColor, Styles::DotDashedLine, false, Styles::Nil, 0.8);
    gridLinesScheme.lineColor.a = 0.15;

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 20);
    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, &writer, &writer, graphs});
}

void SetSchemeHacker() {
    Styles::Color graphTitleFont =  {1,1,1,1};
    Styles::Color graphTicksFont =  {1,1,1,1};
    Styles::Color background =      {0.0,0.0,0.0,1};

    Styles::Color axisColor =       {1,1,1,1};
    Styles::Color tickColor =       {1,1,1,1};

    // https://www.learnui.design/tools/data-color-picker.html
    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{Styles::Color::FromHex("#ff0000"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#00ff00"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#0000ff"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#00ffff"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#ff00ff"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#ffff00"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#7f7fff"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#7fff7f"), Styles::Primitive::SolidLine}
    };

    auto XHairColor = axisColor;
    XHairColor.a = .89;
    auto XHairStyle = Styles::PlotStyle(axisColor);
    XHairStyle.primitive = Styles::DashedLine;
    XHairStyle.thickness = 1;

    auto gridLinesScheme = Styles::PlotStyle(tickColor, Styles::DotDashedLine, false, Styles::Nil, 0.8);
    gridLinesScheme.lineColor.a = 0.15;

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);
    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, &writer, &writer, graphs});
}

void SetSchemePrint () {
    using namespace Styles;

    Color graphTitleFont = {0,0,0,1};
    Color graphTicksFont = {0,0,0,0.75};
    Color background = {1,1,1,1};

    Color axisColor = {0,0,0,1};
    Color tickColor = {.2,.2,.2,1};

    Color black = {0,0,0,1};
    Color graph1a = black;
    Color graph1b = black;
    Color graph1c = {.25 ,.25 ,.25 ,1};

    Color graph1a_fill = {graph1a.rgb(), 1/3.};
    Color graph1b_fill = {graph1b.rgb(), 0.1};
    Color graph1c_fill = {graph1c.rgb(), .25};

    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{graph1a, Styles::Primitive::SolidLine, true, graph1a_fill, 3},
            Styles::PlotStyle{graph1b, Styles::Primitive::SolidLine, true, graph1b_fill, 1},
            Styles::PlotStyle{graph1c, Styles::Primitive::DashedLine, true, graph1c_fill, 3}};

    auto XHairColor = axisColor;
    XHairColor.a = .89;
    auto XHairStyle = Styles::PlotStyle(axisColor);
    XHairStyle.primitive = Styles::DashedLine;
    XHairStyle.thickness = 1;

    auto gridLinesScheme = Styles::PlotStyle(tickColor, Styles::DotDashedLine, false, Styles::Nil, 0.8);
    gridLinesScheme.lineColor.a = 0.15;

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);
    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, &writer, &writer, graphs});
}

void SetSchemeLight () {
    using namespace Styles;

    Color graphTitleFont = {0,0,0,1};
    Color graphTicksFont = {0,0,0,0.75};
    Color background = {1,1,1,1};

    Color axisColor = {0,0,0,1};
    Color tickColor = {.2,.2,.2,1};

    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{Color::FromHex("#003f5c"), Styles::Primitive::SolidLine, FILLED, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#2f4b7c"), Styles::Primitive::SolidLine, FILLED, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#665191"), Styles::Primitive::SolidLine, FILLED, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#a05195"), Styles::Primitive::SolidLine, FILLED, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#d45087"), Styles::Primitive::SolidLine, FILLED, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#f95d6a"), Styles::Primitive::SolidLine, FILLED, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#ff7c43"), Styles::Primitive::SolidLine, FILLED, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#ffa600"), Styles::Primitive::SolidLine, FILLED, Nil, 0.8},
    };

    auto XHairColor = axisColor;
    XHairColor.a = .89;
    auto XHairStyle = Styles::PlotStyle(axisColor);
    XHairStyle.primitive = Styles::DashedLine;
    XHairStyle.thickness = 1;

    auto gridLinesScheme = Styles::PlotStyle(tickColor, Styles::DotDashedLine, false, Styles::Nil, 0.8);
    gridLinesScheme.lineColor.a = 0.15;

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);
    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, &writer, &writer, graphs});
}

void SetSchemeElegant() {
    using namespace Styles;

    Core::Graphics::backgroundColor = {.9,.9,.93,1};
    Core::Graphics::windowBorderColor_inactive = {0.2,0.2,0.2,1};
    Core::Graphics::windowBorderColor_active   = {0. ,0. ,0. ,1};

    Color graphTitleFont = {0,0,0,1};
    Color graphTicksFont = {0,0,0,1};
    Color background = {1,1,1,1};

    Color axisColor = {0,0,0,1};
    Color tickColor = {0,0,0,1};

    fix lineWidth = 1.2f;

    fix fill = FILLED;

    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{Color::FromHex("#ff0000"), Styles::Primitive::SolidLine, fill, Nil, lineWidth},
            Styles::PlotStyle{Color::FromHex("#00ff00"), Styles::Primitive::SolidLine, fill, Nil, lineWidth},
            Styles::PlotStyle{Color::FromHex("#0000ff"), Styles::Primitive::SolidLine, fill, Nil, lineWidth},
            Styles::PlotStyle{Color::FromHex("#ff00ff"), Styles::Primitive::SolidLine, fill, Nil, lineWidth},
            Styles::PlotStyle{Color::FromHex("#00ffff"), Styles::Primitive::SolidLine, fill, Nil, lineWidth},
            Styles::PlotStyle{Color::FromHex("#ffff00"), Styles::Primitive::SolidLine, fill, Nil, lineWidth},
            Styles::PlotStyle{Color::FromHex("#700000"), Styles::Primitive::SolidLine, fill, Nil, lineWidth},
            Styles::PlotStyle{Color::FromHex("#007000"), Styles::Primitive::SolidLine, fill, Nil, lineWidth},
            Styles::PlotStyle{Color::FromHex("#000070"), Styles::Primitive::SolidLine, fill, Nil, lineWidth},
    };

    auto XHairColor = axisColor;
    XHairColor.a = .89;
    auto XHairStyle = Styles::PlotStyle(axisColor);
    XHairStyle.primitive = Styles::Lines;
    XHairStyle.thickness = 1;
    XHairStyle.filled = false;

    auto gridLinesScheme = Styles::PlotStyle(tickColor, Styles::DotDashedLine, false, Styles::Nil, 0.8);
    gridLinesScheme.lineColor.a = 0.15;

    static Core::Graphics::Writer labelsWriter(Resources::fontFileName(19), 24);
    static Core::Graphics::Writer ticksWriter(Resources::fontFileName(18), 24);
    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, &labelsWriter, &ticksWriter, graphs});
}

Styles::ColorScheme_ptr Styles::GetCurrent() {
    if(current == nullptr) Init();

    return current;
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
                                 Core::Graphics::Writer *labelsWriter,
                                 Core::Graphics::Writer *ticksWriter,
                                 std::vector<PlotStyle> plotStyles)
                                 : graphBackground(graphBackground)
                                 , graphNumbersColor(graphNumbersColor)
                                 , graphTitleColor(graphTitleColor)
                                 , axisColor(axisColor)
                                 , majorTickColor(majorTickColor)
                                 , XHairStyle(XHairStyle)
                                 , gridLines(gridStyle)
                                 , labelsWriter(labelsWriter)
                                 , ticksWriter(ticksWriter)
                                 , funcPlotStyles(std::move(plotStyles)) {}

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

Styles::PlotStyle Styles::PlotStyle::permuteColors(bool odd) {

    return Styles::PlotStyle(lineColor.permute(odd), primitive, filled, fillColor.permute(odd), thickness);
}

