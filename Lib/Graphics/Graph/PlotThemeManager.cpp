//
// Created by joao on 30/09/23.
//

#include "PlotThemeManager.h"

#include <memory>
#include "Core/Tools/Resources.h"

#include "3rdParty/imgui/imgui.h"
#include "Core/Backend/BackendManager.h"

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

namespace Slab::Graphics {

    GraphTheme_ptr GetSchemeDark();
    GraphTheme_ptr GetSchemeDark2();
    GraphTheme_ptr GetSchemeHacker();
    GraphTheme_ptr GetSchemePrint();
    GraphTheme_ptr GetSchemeLight();
    GraphTheme_ptr GetSchemeElegant();

    std::map<Str, GraphTheme_ptr(*)()> stylesInitializers;
    std::map<Str, GraphTheme_ptr> loadedStyles;

    Str current;

    GraphTheme_ptr LoadStyle(Str style) {
        return loadedStyles[style] = stylesInitializers[style]();
    }

    PlotThemeManager::PlotThemeManager() : Singleton("Styles manager") {
        stylesInitializers["Dark"]      = GetSchemeDark;
        stylesInitializers["Dark2"]     = GetSchemeDark2;
        stylesInitializers["Hacker"]    = GetSchemeHacker;
        stylesInitializers["Print"]     = GetSchemePrint;
        stylesInitializers["Light"]     = GetSchemeLight;
        stylesInitializers["Elegant"]   = GetSchemeElegant;

        current = "Dark";
        // current = "Light";
        // current = "Print";

        Core::BackendManager::GetGUIBackend().addEventListener(Slab::DummyPointer(*this));;
    }

    GraphTheme_ptr PlotThemeManager::GetCurrent() {
        PlotThemeManager::GetInstance();

        auto style = loadedStyles[current];
        if(style == nullptr)
            style = LoadStyle(current);

        return style;
    }

    bool PlotThemeManager::notifyRender() {
        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("Style")){
                if(ImGui::BeginMenu("Graphs")) {
                    for (auto &stylePair: stylesInitializers) {
                        auto name = stylePair.first;

                        if (ImGui::MenuItem(name.c_str(), nullptr, current == name))
                            current = name;
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }


        return GUIEventListener::notifyRender();
    }


    GraphTheme_ptr GetSchemeDark () {
        Color graphTitleColor   = {1   ,1   ,1   ,1};
        Color graphNumbersColor = {1   ,1   ,1   , .8};
        Color background        = {0.05,0.05,0.08,1};

        Color axisColor =       {1,1,1,.85};
        Color tickColor =       {1,1,1,.75};

        // https://www.learnui.design/tools/data-color-picker.html
        std::vector<PlotStyle> graphs = {
                PlotStyle{Color::FromHex("#004c6d"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#1d617f"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#347691"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#498ca3"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#5fa2b5"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#76b9c7"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#8ed0da"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#a7e7ec"), LinePrimitive::Solid}
        };

        auto XHairColor = axisColor;
        XHairColor.a = .15;
        auto XHairStyle = PlotStyle(axisColor);
        XHairStyle.primitive = Lines;
        XHairStyle.thickness = .5;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::Writer>(Core::Resources::fontFileName(10), 17);

        Graphics::clearColor = {};

        return NewTheme(
                {background, graphNumbersColor, graphTitleColor, axisColor, tickColor, XHairStyle, gridLinesScheme,
                 writer, writer, graphs});

    }

    GraphTheme_ptr GetSchemeDark2 () {
        Color graphTitleFont =  {1,1,1,1};
        Color graphTicksFont =  {1,1,1,.8};
        Color background =      {0.05,0.05,0.08,1};

        Color axisColor =       {1,1,1,.25};
        Color tickColor =       {1,1,1,.25};

        // https://www.learnui.design/tools/data-color-picker.html
        std::vector<PlotStyle> graphs = {
                PlotStyle{Color::FromHex("#003f5c"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{Color::FromHex("#2f4b7c"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{Color::FromHex("#665191"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{Color::FromHex("#a05195"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{Color::FromHex("#d45087"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{Color::FromHex("#f95d6a"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{Color::FromHex("#ff7c43"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{Color::FromHex("#ffa600"), LinePrimitive::Solid, true, Nil, 1.5}
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        XHairStyle.primitive = Dashed;
        XHairStyle.thickness = 1;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::Writer>(Core::Resources::fontFileName(10), 20);
        return NewTheme(
                {background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, writer,
                 writer, graphs});
    }

    GraphTheme_ptr GetSchemeHacker() {
        Color graphTitleFont =  {1,1,1,1};
        Color graphTicksFont =  {1,1,1,1};
        Color background =      {0.0,0.0,0.0,1};

        Color axisColor =       {1,1,1,1};
        Color tickColor =       {1,1,1,1};

        // https://www.learnui.design/tools/data-color-picker.html
        std::vector<PlotStyle> graphs = {
                PlotStyle{Color::FromHex("#ff0000"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#00ff00"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#0000ff"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#00ffff"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#ff00ff"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#ffff00"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#7f7fff"), LinePrimitive::Solid},
                PlotStyle{Color::FromHex("#7fff7f"), LinePrimitive::Solid}
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        XHairStyle.primitive = Dashed;
        XHairStyle.thickness = 1;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::Writer>(Core::Resources::fontFileName(10), 24);
        return NewTheme(
                {background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, writer,
                 writer, graphs});
    }

    GraphTheme_ptr GetSchemePrint () {
        Color graphTitleFont = {0,0,0,1};
        Color graphTicksFont = {0,0,0,0.75};
        Color background = {1,1,1,1};

        Color axisColor = {0,0,0,1};
        Color tickColor = {.2,.2,.2,1};

        Color black = {0,0,0,1};
        Color red = {1, 0, 0, 0.86};
        Color graph1a = black;
        Color graph1b = red;
        Color graph1c = {.25 ,.25 ,.25 ,1};

        Color graph1a_fill = {graph1a.rgb(), 1/3.};
        Color graph1b_fill = {graph1b.rgb(), 0.1};
        Color graph1c_fill = {graph1c.rgb(), .25};

        std::vector<PlotStyle> graphs = {
                PlotStyle{graph1a, LinePrimitive::Solid, true, graph1a_fill, 3},
                PlotStyle{graph1b, LinePrimitive::DotDashed, true, graph1b_fill, 1},
                PlotStyle{graph1c, LinePrimitive::Dashed, true, graph1c_fill, 3}};

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        XHairStyle.primitive = Dashed;
        XHairStyle.thickness = 2;
        XHairStyle.filled = false;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::Writer>(Core::Resources::fontFileName(10), 40);
        return NewTheme(
                {background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, writer,
                 writer, graphs});
    }

    GraphTheme_ptr GetSchemeLight () {
        Color graphTitleFont = {0,0,0,1};
        Color graphTicksFont = {0,0,0,0.75};
        Color background = {1,1,1,1};

        Color axisColor = {0,0,0,1};
        Color tickColor = {.2,.2,.2,1};

        std::vector<PlotStyle> graphs = {
                PlotStyle{Color::FromHex("#003f5c"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{Color::FromHex("#2f4b7c"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{Color::FromHex("#665191"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{Color::FromHex("#a05195"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{Color::FromHex("#d45087"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{Color::FromHex("#f95d6a"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{Color::FromHex("#ff7c43"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{Color::FromHex("#ffa600"), LinePrimitive::Solid, FILLED, Nil, 0.8},
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        XHairStyle.primitive = Dashed;
        XHairStyle.thickness = 1;
        XHairStyle.filled = false;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::Writer>(Core::Resources::fontFileName(10), 24);
        return NewTheme(
                {background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, writer,
                 writer, graphs});
    }

    GraphTheme_ptr GetSchemeElegant() {
        Graphics::clearColor = {.9, .9, .93, 1};
        Graphics::windowBorderColor_inactive = {0.2,0.2,0.2,1};
        Graphics::windowBorderColor_active   = {0. ,0. ,0. ,1};

        Color graphTitleFont = {0,0,0,1};
        Color graphTicksFont = {0,0,0,1};
        Color background = {1,1,1,1};

        Color axisColor = {0,0,0,1};
        Color tickColor = {0,0,0,1};

        fix lineWidth = 1.2f;

        fix fill = FILLED;

        std::vector<PlotStyle> graphs = {
                PlotStyle{Color::FromHex("#ff0000"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{Color::FromHex("#00ff00"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{Color::FromHex("#0000ff"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{Color::FromHex("#ff00ff"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{Color::FromHex("#00ffff"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{Color::FromHex("#ffff00"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{Color::FromHex("#700000"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{Color::FromHex("#007000"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{Color::FromHex("#000070"), LinePrimitive::Solid, fill, Nil, lineWidth},
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        XHairStyle.primitive = Lines;
        XHairStyle.thickness = 1;
        XHairStyle.filled = false;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto labelsWriter = std::make_shared<Graphics::Writer>(Core::Resources::fontFileName(19), 24);
        auto ticksWriter = std::make_shared<Graphics::Writer>(Core::Resources::fontFileName(18), 24);
        return NewTheme({background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme,
                         labelsWriter, ticksWriter, graphs});
    }






} // Math