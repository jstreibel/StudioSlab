//
// Created by joao on 3/25/23.
//

#include "StylesAndColorSchemes.h"
#include "Common/Utils.h"
#include <memory>

Styles::ColorScheme_ptr current;

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
            {graph1a,       Styles::Trace::Solid, true, graph1a_fill, 2.5},
            {graph1b,       Styles::Trace::Solid, true, graph1b_fill, 1},
            {graph1c,       Styles::Trace::Solid, true, graph1c_fill, 1}};

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
            {Styles::Color::FromHex("#004c6d"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#1d617f"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#347691"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#498ca3"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#5fa2b5"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#76b9c7"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#8ed0da"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#a7e7ec"), Styles::Trace::Solid}
    };

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});
}

void SetSchemeDark3 () {
    Styles::Color graphTitleFont =  {1,1,1,1};
    Styles::Color graphTicksFont =  {1,1,1,.8};
    Styles::Color background =      {0.05,0.05,0.08,1};

    Styles::Color axisColor =       {.95,.99,.95,1};
    Styles::Color tickColor =       {.25,.35,.35,1};

    // https://www.learnui.design/tools/data-color-picker.html
    std::vector<Styles::PlotStyle> graphs = {
            {Styles::Color::FromHex("#003f5c"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#2f4b7c"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#665191"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#a05195"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#d45087"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#f95d6a"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#ff7c43"), Styles::Trace::Solid},
            {Styles::Color::FromHex("#ffa600"), Styles::Trace::Solid}
    };

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});
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
            {graph1a,          Styles::Trace::Solid},
            {graph1b,          Styles::Trace::Solid},
            {graph1c,          Styles::Trace::Solid},
            {{.25,.25,.25,1},  Styles::Trace::Solid}};

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
            {graph1a,          Styles::Trace::Solid,  true, graph1a_fill, 3},
            {graph1b,          Styles::Trace::Solid, true, graph1b_fill, 1},
            {graph1c,          Styles::Trace::Dashed,  true, graph1c_fill, 3}};

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
            {graph1a,          Styles::Trace::Solid},
            {graph1b,          Styles::Trace::Solid},
            {graph1c,          Styles::Trace::Solid},
            {{.25,.25,.25,1},  Styles::Trace::Solid}};

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
            {Color::FromHex("#003f5c"),          Styles::Trace::Solid},
            {Color::FromHex("#2f4b7c"),          Styles::Trace::Solid},
            {Color::FromHex("#665191"),          Styles::Trace::Solid},
            {Color::FromHex("#a05195"),          Styles::Trace::Solid},
            {Color::FromHex("#d45087"),          Styles::Trace::Solid},
            {Color::FromHex("#f95d6a"),          Styles::Trace::Solid},
            {Color::FromHex("#ff7c43"),          Styles::Trace::Solid},
            {Color::FromHex("#ffa600"),          Styles::Trace::Solid},
    };

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});
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
            {graph1a,          Styles::Trace::Solid},
            {graph1b,          Styles::Trace::Solid},
            {graph1c,          Styles::Trace::Solid},
            {{.25,.25,.25,1},  Styles::Trace::Solid}};

    current = Styles::NewScheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, graphs});

}


void Styles::Init() {
    //SetSchemeDark();
    SetSchemeDark3();
    //SetSchemePrint();
    //SetSchemeLight2();
}

Styles::ColorScheme_ptr Styles::GetColorScheme() {
    return current;
}

Styles::ColorScheme_ptr Styles::NewScheme(Styles::StyleScheme scheme) {
    { return std::make_shared<Styles::StyleScheme>(scheme); }
}

Styles::StyleScheme::StyleScheme(Color background, Color graphTicksFont, Color graphTitleFont,
                                 Color axisColor, Color majorTickColor, std::vector<PlotStyle> plotStyles)
        : background(background), graphTicksFont(graphTicksFont), graphTitleFont(graphTitleFont),
          axisColor(axisColor), majorTickColor(majorTickColor), funcPlotStyles(plotStyles) {}

Styles::PlotStyle::PlotStyle(Color color, Styles::Trace trace, bool filled,
                             Color lineFill, float lineWidth)
        : lineColor(color), trace(trace), filled(filled), fillColor(lineFill), lineWidth(lineWidth) {
    if(filled && lineFill==Nil) {
        this->fillColor = color;
        this->fillColor.a *= .25;
    }

    stippleFactor = 1;

    switch (trace) {
        case Solid:
            break;
        case Dotted:
            stipplePattern = 0x1111; //BinaryToUInt("o   o   o   o   ", 'o', ' ');
            stippleFactor = 2;
            break;
        case Dashed:
            stipplePattern = Common::BinaryToUInt(" o o o o o o o o", 'o', ' ');
            stippleFactor = 10;
            break;
        case DotDashed:
            stipplePattern = Common::BinaryToUInt("o  ooo  o  ooo  ", 'o', ' ');
            stippleFactor = 2;
            break;
    }
}
