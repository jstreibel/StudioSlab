//
// Created by joao on 3/25/23.
//

#include "StylesAndColorSchemes.h"
#include "Common/Utils.h"
#include <memory>

Styles::ColorScheme_ptr current;

void SetSchemeDark () {
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

void SetSchemeLight () {
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
    SetSchemePrint();
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
