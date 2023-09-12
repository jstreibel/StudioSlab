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

void SetSchemeDark1();
void SetSchemeDark2();
void SetSchemeDark3();

void SetSchemeHacker();

void SetSchemeLight2();

void SetSchemeElegant();

void Styles::Init() {

    // SetSchemeDark1();
    // SetSchemeDark2();
    SetSchemeDark3();

    // SetSchemeHacker();

    // SetSchemeLight2();

    // SetSchemeElegant();
}


void SetSchemeDark1 () {
    Styles::Color graphTitleFont =  {1,1,1,1};
    Styles::Color graphTicksFont =  {1,1,1,.8};
    Styles::Color background =      {0.03,0.05,0.03,1};

    Styles::Color axisColor =       {1,1,1, 0.75};
    Styles::Color tickColor =       {1,.75,.75, 0.5};

    Styles::Color graph1a =         {.65,.65,.99,1};
    Styles::Color graph1b =         {.8,.2,.2, 1};
    Styles::Color graph1c =         {.2,.8,.2, 1};

    Styles::Color graph1a_fill =    graph1a; graph1a_fill.a = .35;
    Styles::Color graph1b_fill =    graph1b; graph1b_fill.a = .15;
    Styles::Color graph1c_fill =    graph1c; graph1c_fill.a = .15;

    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{graph1a, Styles::Primitive::SolidLine, true, graph1a_fill, 2.5},
            Styles::PlotStyle{graph1b, Styles::Primitive::SolidLine, true, graph1b_fill, 1.0},
            Styles::PlotStyle{graph1c, Styles::Primitive::SolidLine, true, graph1c_fill, 1.0},
            Styles::PlotStyle{graph1a.permute(), Styles::Primitive::SolidLine, true, graph1a_fill, 2.5},
            Styles::PlotStyle{graph1b.permute(), Styles::Primitive::SolidLine, true, graph1b_fill, 1.0},
            Styles::PlotStyle{graph1c.permute(), Styles::Primitive::SolidLine, true, graph1c_fill, 1.0}};

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);
    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, &writer, &writer, graphs});
}

void SetSchemeDark2 () {
    Styles::Color graphTitleFont =  {1,1,1,1};
    Styles::Color graphTicksFont =  {1,1,1,.8};
    Styles::Color background =      {0.05,0.05,0.08,1};

    Styles::Color axisColor =       {1,1,1,.25};
    Styles::Color tickColor =       {1,1,1,.25};

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

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);
    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, &writer, &writer, graphs});
}

void SetSchemeDark3 () {
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

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 20);
    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, &writer, &writer, graphs});
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

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);
    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, &writer, &writer, graphs});
}

void SetSchemeBWDark () {
    using namespace Styles;

    Color graphTitleFont = {1,1,1,1};
    Color graphTicksFont = {1,1,1,0.75};
    Color background = {0,0,0,1};

    Color axisColor = {1,1,1,1};
    Color tickColor = {.9,.9,.9,1};

    Color graph1a = {1,1,1,1};
    Color graph1b = {.75,.75,.75,1};
    Color graph1c = {.5,.5,.5,1};

    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{graph1a,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{graph1b,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{graph1c,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{{.25,.25,.25,1},  Styles::Primitive::SolidLine}};

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);
    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, &writer, &writer, graphs});
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

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, &writer, &writer, graphs});
}

void SetSchemeLight1 () {
    using namespace Styles;

    Color graphTitleFont = {0,0,0,1};
    Color graphTicksFont = {0,0,0,0.75};
    Color background = {1,1,1,1};

    Color axisColor = {0,0,0,1};
    Color tickColor = {.2,.2,.2,1};

    Color graph1a = {.25,.3,.6,1};
    Color graph1b = {.3,.6,.25,1};
    Color graph1c = {.6,.3,.25,1};

    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{graph1a,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{graph1b,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{graph1c,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{{.25,.25,.25,1},  Styles::Primitive::SolidLine}};

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, &writer, &writer, graphs});
}

void SetSchemeLight2 () {
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

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, &writer, &writer, graphs});
}

void SetSchemeTest () {
    using namespace Styles;

    Color graphTitleFont = {1.f, .4f, 1.f, 0.6f};
    Color graphTicksFont = {1.f, .0f, 0.f, 1.0f};
    Color background = {0.35, 0.35, 0.7, 1.0};

    Color axisColor = {0,0,0,1};
    Color tickColor = {0,0,1,1};

    Color graph1a = {.5,.5,.5,1};
    Color graph1b = {1,1,1,1};
    Color graph1c = {.751,.75,.75,1};

    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{graph1a,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{graph1b,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{graph1c,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{{.25,.25,.25,1},  Styles::Primitive::SolidLine}};

    static Core::Graphics::Writer writer(Resources::fontFileName(10), 24);

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, &writer, &writer, graphs});

}

void SetSchemeElegant() {
    using namespace Styles;

    Core::Graphics::backgroundColor = {.9,.9,.93,1};
    Core::Graphics::windowBorderColor = {0,0,0,1};

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

    static Core::Graphics::Writer labelsWriter(Resources::fontFileName(19), 24);
    static Core::Graphics::Writer ticksWriter(Resources::fontFileName(18), 24);

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, &labelsWriter, &ticksWriter, graphs});
}

Styles::ColorScheme_ptr Styles::GetCurrent() {
    return current;
}

Styles::ColorScheme_ptr Styles::NewScheme(const Styles::StyleScheme& scheme) {
    { return std::make_shared<Styles::StyleScheme>(scheme); }
}

Styles::StyleScheme::StyleScheme(Color windowBackground,
                                 Color graphTicksFont,
                                 Color graphTitleFont,
                                 Color axisColor,
                                 Color majorTickColor,
                                 Core::Graphics::Writer *labelsWriter,
                                 Core::Graphics::Writer *ticksWriter,
                                 std::vector<PlotStyle> plotStyles)
                                 : graphBackground(windowBackground)
                                 , graphTicksFont(graphTicksFont)
                                 , graphTitleFont(graphTitleFont)
                                 , axisColor(axisColor)
                                 , majorTickColor(majorTickColor)
                                 , labelsWriter(labelsWriter)
                                 , ticksWriter(ticksWriter)
                                 , funcPlotStyles(std::move(plotStyles)) {}

Styles::PlotStyle::PlotStyle(Color color, Styles::Primitive primitive, bool filled,
                             Color lineFill, float lineWidth)
        : lineColor(color), primitive(primitive), filled(filled), fillColor(lineFill), thickness(lineWidth) {
    if(filled && lineFill==Nil) {
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

