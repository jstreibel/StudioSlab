//
// Created by joao on 3/25/23.
//

#include "StylesAndColorSchemes.h"
#include "Utils/Utils.h"
#include <memory>
#include <utility>

#define FILLED_GRAPH true

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

void SetSchemeLight2();

void Styles::Init() {

    SetSchemeDark3();

    // SetSchemeLight2();
}


void SetSchemeDark1 () {
    Styles::Color graphTitleFont =  {1,1,1,1};
    Styles::Color graphTicksFont =  {1,1,1,.8};
    Styles::Color background =      {0,0,0,1};

    Styles::Color axisColor =       {1,1,1,1};
    Styles::Color tickColor =       {1,1,1,1};

    Styles::Color graph1a =         {.65,.65,.99,1};
    Styles::Color graph1b =         {.8,.2,.2, 1};
    Styles::Color graph1c =         {.2,.8,.2, 1};

    Styles::Color graph1a_fill =    graph1a; graph1a_fill.a = .35;
    Styles::Color graph1b_fill =    graph1b; graph1b_fill.a = .15;
    Styles::Color graph1c_fill =    graph1c; graph1c_fill.a = .15;

    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{graph1a, Styles::Primitive::SolidLine, true, graph1a_fill, 2.5},
            Styles::PlotStyle{graph1b, Styles::Primitive::SolidLine, true, graph1b_fill, 1.0},
            Styles::PlotStyle{graph1c, Styles::Primitive::SolidLine, true, graph1c_fill, 1.0}};

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});
}

void SetSchemeDark2 () {
    Styles::Color graphTitleFont =  {1,1,1,1};
    Styles::Color graphTicksFont =  {1,1,1,.8};
    Styles::Color background =      {0,0,0,1};

    Styles::Color axisColor =       {1,1,1,1};
    Styles::Color tickColor =       {1,1,1,1};

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

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});
}

void SetSchemeDark3 () {
    Styles::Color graphTitleFont =  {1,1,1,1};
    Styles::Color graphTicksFont =  {1,1,1,.8};
    Styles::Color background =      {0.05,0.05,0.08,1};

    Styles::Color axisColor =       {1,1,1,.25};
    Styles::Color tickColor =       {1,1,1,.25};

    // https://www.learnui.design/tools/data-color-picker.html
    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{Styles::Color::FromHex("#003f5c"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#2f4b7c"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#665191"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#a05195"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#d45087"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#f95d6a"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#ff7c43"), Styles::Primitive::SolidLine},
            Styles::PlotStyle{Styles::Color::FromHex("#ffa600"), Styles::Primitive::SolidLine}
    };

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});
    current->deepBackground = {0.1,0.1,0.16,1};;
    current->windowBorder = current->deepBackground.inverse();
}

void SetSchemeBWDark () {
    using namespace Styles;

    Color graphTitleFont = {1,1,1,1};
    Color graphTicksFont = {1,1,1,0.75};
    Color background = {0,0,0,1};

    Color axisColor = {1,1,1,1};
    Color tickColor = {.9,.9,.9,1};

    Color defaultGraphColor = {.9,.9,.9,1};

    Color graph1a = {1,1,1,1};
    Color graph1b = {.75,.75,.75,1};
    Color graph1c = {.5,.5,.5,1};

    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{graph1a,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{graph1b,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{graph1c,          Styles::Primitive::SolidLine},
            Styles::PlotStyle{{.25,.25,.25,1},  Styles::Primitive::SolidLine}};

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});
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

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});
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

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});
}

void SetSchemeLight2 () {
    using namespace Styles;

    Color graphTitleFont = {0,0,0,1};
    Color graphTicksFont = {0,0,0,0.75};
    Color background = {1,1,1,1};

    Color axisColor = {0,0,0,1};
    Color tickColor = {.2,.2,.2,1};

    std::vector<Styles::PlotStyle> graphs = {
            Styles::PlotStyle{Color::FromHex("#003f5c"), Styles::Primitive::SolidLine, FILLED_GRAPH, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#2f4b7c"), Styles::Primitive::SolidLine, FILLED_GRAPH, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#665191"), Styles::Primitive::SolidLine, FILLED_GRAPH, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#a05195"), Styles::Primitive::SolidLine, FILLED_GRAPH, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#d45087"), Styles::Primitive::SolidLine, FILLED_GRAPH, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#f95d6a"), Styles::Primitive::SolidLine, FILLED_GRAPH, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#ff7c43"), Styles::Primitive::SolidLine, FILLED_GRAPH, Nil, 0.8},
            Styles::PlotStyle{Color::FromHex("#ffa600"), Styles::Primitive::SolidLine, FILLED_GRAPH, Nil, 0.8},
    };

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});
    current->deepBackground = {0.925,0.925,1.,1};
    current->windowBorder = {0, 0, 0, 1};
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

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});

}

Styles::ColorScheme_ptr Styles::GetColorScheme() {
    return current;
}

Styles::ColorScheme_ptr Styles::NewScheme(const Styles::StyleScheme& scheme) {
    { return std::make_shared<Styles::StyleScheme>(scheme); }
}

Styles::StyleScheme::StyleScheme(Color windowBackground, Color graphTicksFont, Color graphTitleFont,
                                 Color axisColor, Color majorTickColor, std::vector<PlotStyle> plotStyles)
        : windowBackground(windowBackground), graphTicksFont(graphTicksFont), graphTitleFont(graphTitleFont),
          axisColor(axisColor), majorTickColor(majorTickColor), funcPlotStyles(std::move(plotStyles)) {}

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

