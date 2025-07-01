//
// Created by joao on 30/09/23.
//

#include "PlotThemeManager.h"

#include <memory>

#include "Core/Tools/Resources.h"

#include "Core/Tools/Log.h"
#include "Graphics/OpenGL/WriterOpenGL.h"
#include "Core/SlabCore.h"

#define FILLED true

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
    GraphTheme_ptr GetSchemePrintSmall();
    GraphTheme_ptr GetSchemePrint();
    GraphTheme_ptr GetSchemeLight();
    GraphTheme_ptr GetSchemeElegant();

    std::map<Str, GraphTheme_ptr(*)()> stylesInitializers = {{"Dark",       GetSchemeDark},
                                                             {"Dark2",      GetSchemeDark2},
                                                             {"Hacker",     GetSchemeHacker},
                                                             {"Print",      GetSchemePrint},
                                                             {"PrintSmall", GetSchemePrintSmall},
                                                             {"Light",      GetSchemeLight},
                                                             {"Elegant",    GetSchemeElegant}};
    std::map<Str, GraphTheme_ptr> loadedStyles;

    Str PlotThemeManager::GetDefault() {
        // return "Elegant";
        return "Dark2";
        // return "PrintSmall";
    }

    GraphTheme_ptr LoadStyle(const Str& style) {
        auto sty = loadedStyles[style];
        if(sty == nullptr) {
            auto initializer = stylesInitializers[style];

            if(initializer != nullptr) {
                sty = initializer();
                loadedStyles[style] = sty;
                Core::Log::Info() << "Loaded plotting theme '" << style << "'." << Core::Log::Flush;
            } else {
                auto themes = Slab::Graphics::PlotThemeManager::GetThemes();
                Str available_themes;
                for(auto &theme : themes)
                    available_themes += Str("'") + theme + "', ";
                Core::Log::Warning() << "Trying to set plotting theme to '" << style
                    << "', but theme couldn't be found. Available themes are: " << available_themes << Core::Log::Flush;

                PlotThemeManager::GetInstance().current = PlotThemeManager::GetDefault();
            }
        }

        if(sty != nullptr && style != PlotThemeManager::GetInstance().current) {
            PlotThemeManager::GetInstance().current = style;
        }

        return sty;
    }

    Pointer<PlotThemeManager> mePointer=nullptr;
    PlotThemeManager::PlotThemeManager()
    : Singleton("Styles manager") {
        Core::LoadModule("GUI");

        // TODO isso eh gambiarra:
        mePointer = Naked(*this);
        GetGraphicsBackend()->GetMainSystemWindow()->AddEventListener(mePointer);
    }

    GraphTheme_ptr PlotThemeManager::GetCurrent() {
        // initialize, if not yet. This only serves so that theme choice shows up in main menu bar.
        GetInstance();

        if(PlotThemeManager::GetInstance().current.empty()) {
            PlotThemeManager::GetInstance().current = GetDefault();
        }

        auto style = loadedStyles[PlotThemeManager::GetInstance().current];
        if(style == nullptr) {
            style = LoadStyle(PlotThemeManager::GetInstance().current);
        }

        return style;
    }

    bool PlotThemeManager::NotifyRender() {
        auto gui_context = GetGraphicsBackend()->GetMainSystemWindow()->GetGUIContext();

        if(gui_context != nullptr) {
            Vector<MainMenuLeafEntry> entries;
            for (auto &stylePair: stylesInitializers) {
                auto name = stylePair.first;
                entries.emplace_back(name, "", current == name);
            }

            gui_context->AddMainMenuItem(MainMenuItem{
                    MainMenuLocation{"Style", "Graphs"},
                    entries,
                    [](const Str &name) {
                        PlotThemeManager::GetInstance().current = name;
                    }
            });
        }

        return FPlatformWindowEventListener::NotifyRender();
    }

    bool PlotThemeManager::SetTheme(const Str& theme) {
        if(stylesInitializers[theme] == nullptr) {
            Core::Log::Warning() << "While trying to set theme to '" << theme << "'. "
                                 << "Falling back to '" << GetDefault() << "'." << Core::Log::Flush;

            PlotThemeManager::GetInstance().current = GetDefault();

            return false;
        } else {
            PlotThemeManager::GetInstance().current = theme;
        }

        return true;
    }

    StrVector PlotThemeManager::GetThemes() {
        StrVector vecky;
        for(auto &a : stylesInitializers)
            vecky.push_back(a.first);

        return vecky;
    }

    GraphTheme_ptr GetSchemeDark () {
        FColor graphTitleColor   = {1   ,1   ,1   ,1};
        FColor graphNumbersColor = {1   ,1   ,1   , .8};
        FColor background        = {0.05,0.05,0.08,1};

        FColor axisColor =       {1,1,1,.85};
        FColor tickColor =       {1,1,1,.75};

        // https://www.learnui.design/tools/data-color-picker.html
        Vector<PlotStyle> graphs = {
                PlotStyle{FColor::FromHex("#004c6d"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#1d617f"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#347691"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#498ca3"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#5fa2b5"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#76b9c7"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#8ed0da"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#a7e7ec"), LinePrimitive::Solid}
        };

        auto XHairColor = axisColor;
        XHairColor.a = .15;
        auto XHairStyle = PlotStyle(axisColor);
        XHairStyle.setPrimitive(Lines);
        XHairStyle.thickness = .5;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(10), 17);

        WindowStyle::windowBGColor = {};

        return New<PlottingTheme>(PlottingTheme
                {background, graphNumbersColor, graphTitleColor, axisColor, tickColor, XHairStyle, gridLinesScheme, gridLinesScheme,
                 writer, writer, graphs});

    }

    GraphTheme_ptr GetSchemeDark2 () {
        const FColor graphTitleFont =  {1,1,1,1};
        const FColor graphTicksFont =  {1,1,1,.8};
        const FColor background =      {0.05,0.05,0.08,1};

        const FColor axisColor =       {1,1,1,.25};
        const FColor tickColor =       {1,1,1,.25};

        // https://www.learnui.design/tools/data-color-picker.html
        const Vector<PlotStyle> graphs = {
                PlotStyle{FColor::FromHex("#003f5c"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#2f4b7c"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#665191"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#a05195"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#d45087"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#f95d6a"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#ff7c43"), LinePrimitive::Solid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#ffa600"), LinePrimitive::Solid, true, Nil, 1.5}
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        //XHairStyle.primitive = Dashed;
        XHairStyle.setPrimitive(Lines);

        XHairStyle.thickness = 1;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;
        gridLinesScheme.thickness = 1;

        auto writer = std::make_shared<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(10), 20);
        return New<PlottingTheme>(PlottingTheme
                {background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle,
                 gridLinesScheme, gridLinesScheme, writer,
                 writer, graphs});
    }

    GraphTheme_ptr GetSchemeHacker() {
        FColor graphTitleFont =  {1,1,1,1};
        FColor graphTicksFont =  {1,1,1,1};
        FColor background =      {0.0,0.0,0.0,1};

        FColor axisColor =       {1,1,1,1};
        FColor tickColor =       {1,1,1,1};

        // https://www.learnui.design/tools/data-color-picker.html
        Vector<PlotStyle> graphs = {
                PlotStyle{FColor::FromHex("#ff0000"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#00ff00"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#0000ff"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#00ffff"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#ff00ff"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#ffff00"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#7f7fff"), LinePrimitive::Solid},
                PlotStyle{FColor::FromHex("#7fff7f"), LinePrimitive::Solid}
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        //XHairStyle.primitive = Dashed;
        XHairStyle.setPrimitive(Lines);
        XHairStyle.thickness = 1;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(10), 24);
        return New<PlottingTheme>(PlottingTheme
                {background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, gridLinesScheme, writer,
                 writer, graphs});
    }

    GraphTheme_ptr GetSchemePrint () {
        FColor graphTitleFont = {0,0,0,1};
        FColor graphTicksFont = {0,0,0,0.75};
        FColor background = {1,1,1,1};

        FColor axisColor = {0,0,0,1};
        FColor tickColor = {.2,.2,.2,1};

        FColor black = {0,0,0,1};
        FColor red = {1, 0, 0, 0.86};
        FColor graph1a = black;
        FColor graph1b = red;
        FColor graph1c = {.25 ,.25 ,.25 ,1};

        FColor graph1a_fill = {graph1a.rgb(), 1/3.f};
        FColor graph1b_fill = {graph1b.rgb(), 0.1f};
        FColor graph1c_fill = {graph1c.rgb(), .25f};

        Vector<PlotStyle> graphs = {
                PlotStyle{graph1a, LinePrimitive::Solid, true, graph1a_fill, 3},
                PlotStyle{graph1b, LinePrimitive::DotDashed, true, graph1b_fill, 1},
                PlotStyle{graph1c, LinePrimitive::Dashed, true, graph1c_fill, 3}};

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        //XHairStyle.primitive = Dashed;
        XHairStyle.setPrimitive(Lines);
        XHairStyle.thickness = 2;
        XHairStyle.filled = false;

        auto gridLinesScheme = PlotStyle(tickColor, Dashed, false, Nil, 1);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(10), 40);
        return New<PlottingTheme>(PlottingTheme
                {background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle,
gridLinesScheme, gridLinesScheme, writer,
                 writer, graphs});
    }

    GraphTheme_ptr GetSchemePrintSmall() {
        auto scheme = GetSchemePrint();

        scheme->hAxisPaddingInPixels = 120;
        scheme->minorGridLines.thickness = 0.8;
        scheme->minorGridLines.setStippleFactor(scheme->minorGridLines.getStippleFactor()*5);
        scheme->hTickWidthInPixels = 5;
        scheme->vTickHeightinPixels = 15;

        auto writer = std::make_shared<
Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(10), 90);
        scheme->TicksWriter = writer;
        scheme->LabelsWriter = writer;

        return scheme;
    }

    GraphTheme_ptr GetSchemeLight () {
        FColor graphTitleFont = {0,0,0,1};
        FColor graphTicksFont = {0,0,0,0.75};
        FColor background = {1,1,1,1};

        FColor axisColor = {0,0,0,1};
        FColor tickColor = {.2,.2,.2,1};

        Vector<PlotStyle> graphs = {
                PlotStyle{FColor::FromHex("#003f5c"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#2f4b7c"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#665191"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#a05195"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#d45087"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#f95d6a"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#ff7c43"), LinePrimitive::Solid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#ffa600"), LinePrimitive::Solid, FILLED, Nil, 0.8},
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        //XHairStyle.primitive = Dashed;
        XHairStyle.setPrimitive(Lines);
        XHairStyle.thickness = 1;
        XHairStyle.filled = false;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(10), 24);

        return New<PlottingTheme>(PlottingTheme
                {background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, gridLinesScheme, writer,
                 writer, graphs});
    }

    GraphTheme_ptr GetSchemeElegant() {
        WindowStyle::windowBGColor = {.9, .9, .93, 1};
        WindowStyle::windowBorderColor_inactive = {0.2,0.2,0.2,1};
        WindowStyle::windowBorderColor_active   = {0. ,0. ,0. ,1};

        FColor graphTitleFont = {0,0,0,1};
        FColor graphTicksFont = {0,0,0,1};
        FColor background = {1,1,1,1};

        FColor axisColor = {0,0,0,1};
        FColor tickColor = {0,0,0,1};

        fix lineWidth = 1.2f;

        fix fill = FILLED;

        Vector<PlotStyle> graphs = {
                PlotStyle{FColor::FromHex("#ff0000"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#00ff00"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#0000ff"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#ff00ff"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#00ffff"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#ffff00"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#700000"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#007000"), LinePrimitive::Solid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#000070"), LinePrimitive::Solid, fill, Nil, lineWidth},
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        XHairStyle.setPrimitive(Lines);
        XHairStyle.thickness = 1;
        XHairStyle.filled = false;

        auto gridLinesScheme = PlotStyle(tickColor, DotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        // auto labelsWriter = std::make_shared<Graphics::OpenGL::WriterOpenGL>(Core::Resources::fontFileName(19), 24);
        auto ticksWriter = New<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(18), 24);
        auto& labelsWriter = ticksWriter;

        return New<PlottingTheme>(PlottingTheme{background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, gridLinesScheme,
                         labelsWriter, ticksWriter, graphs});
    }






} // Math