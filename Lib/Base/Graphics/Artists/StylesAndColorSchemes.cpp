//
// Created by joao on 3/25/23.
//

#include "StylesAndColorSchemes.h"
#include <memory>


Styles::ColorScheme_ptr current;

void SetSchemeDark () {
    Color graphTitleFont = {1,1,1,1};
    Color graphTicksFont = {1,1,1,.8};
    Color background = {0,0,0,1};

    Color axisColor = {1,1,1,1};
    Color tickColor = {1,1,1,1};

    Color defaultGraphColor = {.9,.9,.9,1};

    Color graph1a = {.65,.65,.99,1};
    Color graph1b = {.8,.2,.2, 1};
    Color graph1c = {.2,.8,.2, 1};

    std::vector<Color> graphs = {graph1a, graph1b, graph1c, {.0,.0,.9,1}};

    current = std::make_shared<Styles::ColorScheme>(background, graphTicksFont, graphTitleFont,
                                                       axisColor, tickColor, graphs);
}

void SetSchemeBWDark () {

    Color graphTitleFont = {1,1,1,1};
    Color graphTicksFont = {1,1,1,0.75};
    Color background = {0,0,0,1};

    Color axisColor = {1,1,1,1};
    Color tickColor = {.9,.9,.9,1};

    Color defaultGraphColor = {.9,.9,.9,1};

    Color graph1a = {1,1,1,1};
    Color graph1b = {.75,.75,.75,1};

    std::vector<Color> graphs = {graph1a, graph1b, {.5,.5,.5,1}, {.25,.25,.25,1}};

    current = std::make_shared<Styles::ColorScheme>(background, graphTicksFont, graphTitleFont,
                                                    axisColor, tickColor, graphs);
}

void SetSchemePrint () {

    Color graphTitleFont = {0,0,0,1};
    Color graphTicksFont = {0,0,0,0.75};
    Color background = {1,1,1,1};

    Color axisColor = {0,0,0,1};
    Color tickColor = {.2,.2,.2,1};

    Color defaultGraphColor = {.1,.1,.1,1};

    Color graph1a = {0,0,0,1};
    Color graph1b = {.25,.25,.25,1};

    std::vector<Color> graphs = {graph1a, graph1b, {.5,.5,.5,1}, {.75,.75,.75,1}};

    current = std::make_shared<Styles::ColorScheme>(background, graphTicksFont, graphTitleFont,
                                                    axisColor, tickColor, graphs);
}

void SetSchemeLight () {

    Color graphTitleFont = {0,0,0,1};
    Color graphTicksFont = {0,0,0,0.75};
    Color background = {1,1,1,1};

    Color axisColor = {0,0,0,1};
    Color tickColor = {.2,.2,.2,1};

    Color defaultGraphColor = {.1,.1,.1,1};

    Color graph1a = {.25,.3,.6,1};
    Color graph1b = {.3,.6,.25,1};

    std::vector<Color> graphs = {graph1a, graph1b, {.6,.25,.3,1}, {.75,.75,.75,1}};

    current = std::make_shared<Styles::ColorScheme>(background, graphTicksFont, graphTitleFont,
                                                    axisColor, tickColor, graphs);
}

void SetSchemeTest () {

    Color graphTitleFont = {1.f, .4f, 1.f, 0.6f};
    Color graphTicksFont = {1.f, .0f, 0.f, 1.0f};
    Color background = {0.35, 0.35, 0.7, 1.0};

    Color axisColor = {0,0,0,1};
    Color tickColor = {0,0,1,1};

    Color defaultGraphColor = {.9,.9,.9,1};

    Color graph1a = {.5,.5,.5,1};
    Color graph1b = {1,1,1,1};

    std::vector<Color> graphs = {graph1a, graph1b, {.6,.25,.3,1}, {.75,.75,.75,1}};

    current = std::make_shared<Styles::ColorScheme>(background, graphTicksFont, graphTitleFont,
                                                    axisColor, tickColor, graphs);

}


void Styles::Init() {
    SetSchemeDark();
}

Styles::ColorScheme_ptr Styles::GetColorScheme() {
    return current;
}

Styles::ColorScheme::ColorScheme(Color background, Color graphTicksFont, Color graphTitleFont,
                                 Color axisColor, Color majorTickColor, std::vector<Color> plotColors)
        : background(background), graphTicksFont(graphTicksFont), graphTitleFont(graphTitleFont),
          axisColor(axisColor), majorTickColor(majorTickColor), plotColors(plotColors) {}
