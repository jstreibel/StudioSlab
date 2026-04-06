//
// Created by joao on 30/09/23.
//

#include "PlotThemeManager.h"

#include <initializer_list>
#include <memory>
#include <ranges>

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
    GraphTheme_ptr GetSchemeGuiNativeLight();
    GraphTheme_ptr GetSchemeGuiNativeDark();
    GraphTheme_ptr GetSchemeGuiDarkRed();
    GraphTheme_ptr GetSchemeGuiStudioSlab();
    GraphTheme_ptr GetSchemeGuiScientificSlate();
    GraphTheme_ptr GetSchemeGuiScientificPaper();
    GraphTheme_ptr GetSchemeGuiBlueprintNight();

    auto ThemeInitializers() -> std::map<Str, GraphTheme_ptr(*)()> & {
        static std::map<Str, GraphTheme_ptr(*)()> initializers = {
            {"Dark", GetSchemeDark},
            {"Dark2", GetSchemeDark2},
            {"Hacker", GetSchemeHacker},
            {"Print", GetSchemePrint},
            {"PrintSmall", GetSchemePrintSmall},
            {"Light", GetSchemeLight},
            {"Elegant", GetSchemeElegant},
            {"GUI Native Light", GetSchemeGuiNativeLight},
            {"GUI Native Dark", GetSchemeGuiNativeDark},
            {"GUI Dark Red", GetSchemeGuiDarkRed},
            {"GUI StudioSlab", GetSchemeGuiStudioSlab},
            {"GUI Scientific Slate", GetSchemeGuiScientificSlate},
            {"GUI Scientific Paper", GetSchemeGuiScientificPaper},
            {"GUI Blueprint Night", GetSchemeGuiBlueprintNight}
        };
        return initializers;
    }

    auto LoadedThemes() -> std::map<Str, GraphTheme_ptr> & {
        static std::map<Str, GraphTheme_ptr> loadedThemes;
        return loadedThemes;
    }

    Str FPlotThemeManager::GetDefault() {
        // return "Elegant";
        return "Dark2";
        // return "PrintSmall";
    }

    GraphTheme_ptr LoadStyle(const Str& style) {
        auto &loadedStyles = LoadedThemes();
        auto &initializers = ThemeInitializers();

        auto sty = loadedStyles[style];
        if (sty == nullptr) {
            const auto initializerIt = initializers.find(style);
            const auto initializer = initializerIt != initializers.end() ? initializerIt->second : nullptr;

            if (initializer != nullptr) {
                sty = initializer();
                loadedStyles[style] = sty;
                Core::FLog::Info() << "Loaded plotting theme '" << style << "'." << Core::FLog::Flush;
            } else {
                auto themes = Slab::Graphics::FPlotThemeManager::GetThemes();
                Str available_themes;
                for(auto &theme : themes)
                    available_themes += Str("'") + theme + "', ";
                Core::FLog::Warning() << "Trying to set plotting theme to '" << style
                    << "', but theme couldn't be found. Available themes are: " << available_themes << Core::FLog::Flush;

                FPlotThemeManager::GetInstance().current = FPlotThemeManager::GetDefault();
            }
        }

        if(sty != nullptr && style != FPlotThemeManager::GetInstance().current) {
            FPlotThemeManager::GetInstance().current = style;
        }

        return sty;
    }

    FPlotThemeManager::FPlotThemeManager()
    : FSingleton("Styles manager") {
    }

    GraphTheme_ptr FPlotThemeManager::GetCurrent() {
        // initialize, if not yet. This only serves so that theme choice shows up in main menu bar.
        GetInstance();

        if(FPlotThemeManager::GetInstance().current.empty()) {
            FPlotThemeManager::GetInstance().current = GetDefault();
        }

        auto &loadedStyles = LoadedThemes();
        auto style = loadedStyles[FPlotThemeManager::GetInstance().current];
        if(style == nullptr) {
            style = LoadStyle(FPlotThemeManager::GetInstance().current);
        }

        return style;
    }

    bool FPlotThemeManager::NotifyRender(const FPlatformWindow& PlatformWindow) {
        /*
        auto GuiContext = PlatformWindow.GetGUIContext();

        if(GuiContext != nullptr) {
            Vector<MainMenuLeafEntry> entries;
            for (const auto& Name : ThemeInitializers() | std::views::keys) {
                entries.emplace_back(Name, "", current == Name);
            }

            GuiContext->AddMainMenuItem(MainMenuItem{
                    MainMenuLocation{"Style", "Graphs"},
                    entries,
                    [](const Str &name) {
                        FPlotThemeManager::GetInstance().current = name;
                    }
            });
        }
        */

        return FPlatformWindowEventListener::NotifyRender(PlatformWindow);
    }

    bool FPlotThemeManager::SetTheme(const Str& theme) {
        const auto &initializers = ThemeInitializers();
        const auto found = initializers.find(theme);
        if (found == initializers.end() || found->second == nullptr) {
            Core::FLog::Warning() << "While trying to set theme to '" << theme << "'. "
                                 << "Falling back to '" << GetDefault() << "'." << Core::FLog::Flush;

            FPlotThemeManager::GetInstance().current = GetDefault();

            return false;
        } else {
            FPlotThemeManager::GetInstance().current = theme;
        }

        return true;
    }

    StrVector FPlotThemeManager::GetThemes() {
        const auto &initializers = ThemeInitializers();
        StrVector Vecky;
        for (const auto &a : initializers)
            Vecky.push_back(a.first);

        return Vecky;
    }

    namespace {

        auto BuildPlotStyles(
            const std::initializer_list<const char*> colorsHex,
            const float thickness,
            const float fillAlpha,
            const bool filled = FILLED) -> Vector<PlotStyle>
        {
            Vector<PlotStyle> styles;
            styles.reserve(colorsHex.size());

            for (const auto* hexColor : colorsHex)
            {
                const auto lineColor = FColor::FromHex(hexColor);
                const auto fillColor = FColor(lineColor.rgb(), fillAlpha);
                styles.emplace_back(PlotStyle{lineColor, LinePrimitive::PlottingSolid, filled, fillColor, thickness});
            }

            return styles;
        }

        auto BuildGuiAlignedScheme(
            const FColor& graphTitleFont,
            const FColor& graphTicksFont,
            const FColor& background,
            const FColor& axisColor,
            const FColor& tickColor,
            const Vector<PlotStyle>& graphs,
            const int fontSize,
            const float gridAlpha,
            const float gridThickness = 1.0f,
            const float crossHairThickness = 1.0f) -> GraphTheme_ptr
        {
            auto xHairStyle = PlotStyle(axisColor);
            xHairStyle.setPrimitive(Lines);
            xHairStyle.thickness = crossHairThickness;
            xHairStyle.filled = false;

            auto gridLinesScheme = PlotStyle(tickColor, PlottingDotDashed, false, Nil, gridThickness);
            gridLinesScheme.lineColor.a = gridAlpha;

            auto writer = std::make_shared<Graphics::OpenGL::FWriterOpenGL>(
                Core::Resources::GetIndexedFontFileName(10),
                fontSize);

            return New<PlottingTheme>(PlottingTheme{
                background, graphTicksFont, graphTitleFont, axisColor, tickColor,
                xHairStyle, gridLinesScheme, gridLinesScheme, writer, writer, graphs});
        }

    } // namespace

    GraphTheme_ptr GetSchemeDark () {
        FColor graphTitleColor   = {1   ,1   ,1   ,1};
        FColor graphNumbersColor = {1   ,1   ,1   , .8};
        FColor background        = {0.05,0.05,0.08,1};

        FColor axisColor =       {1,1,1,.85};
        FColor tickColor =       {1,1,1,.75};

        // https://www.learnui.design/tools/data-color-picker.html
        Vector<PlotStyle> graphs = {
                PlotStyle{FColor::FromHex("#004c6d"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#1d617f"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#347691"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#498ca3"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#5fa2b5"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#76b9c7"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#8ed0da"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#a7e7ec"), LinePrimitive::PlottingSolid}
        };

        auto XHairColor = axisColor;
        XHairColor.a = .15;
        auto XHairStyle = PlotStyle(axisColor);
        XHairStyle.setPrimitive(Lines);
        XHairStyle.thickness = .5;

        auto gridLinesScheme = PlotStyle(tickColor, PlottingDotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(10), 17);

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
                PlotStyle{FColor::FromHex("#003f5c"), LinePrimitive::PlottingSolid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#2f4b7c"), LinePrimitive::PlottingSolid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#665191"), LinePrimitive::PlottingSolid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#a05195"), LinePrimitive::PlottingSolid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#d45087"), LinePrimitive::PlottingSolid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#f95d6a"), LinePrimitive::PlottingSolid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#ff7c43"), LinePrimitive::PlottingSolid, true, Nil, 1.5},
                PlotStyle{FColor::FromHex("#ffa600"), LinePrimitive::PlottingSolid, true, Nil, 1.5}
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        //XHairStyle.primitive = Dashed;
        XHairStyle.setPrimitive(Lines);

        XHairStyle.thickness = 1;

        auto gridLinesScheme = PlotStyle(tickColor, PlottingDotDashed, false, Nil, 0.8);
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
                PlotStyle{FColor::FromHex("#ff0000"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#00ff00"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#0000ff"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#00ffff"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#ff00ff"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#ffff00"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#7f7fff"), LinePrimitive::PlottingSolid},
                PlotStyle{FColor::FromHex("#7fff7f"), LinePrimitive::PlottingSolid}
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        //XHairStyle.primitive = Dashed;
        XHairStyle.setPrimitive(Lines);
        XHairStyle.thickness = 1;

        auto gridLinesScheme = PlotStyle(tickColor, PlottingDotDashed, false, Nil, 0.8);
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
                PlotStyle{graph1a, LinePrimitive::PlottingSolid, true, graph1a_fill, 3},
                PlotStyle{graph1b, LinePrimitive::PlottingDotDashed, true, graph1b_fill, 1},
                PlotStyle{graph1c, LinePrimitive::PlottingDashed, true, graph1c_fill, 3}};

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        //XHairStyle.primitive = Dashed;
        XHairStyle.setPrimitive(Lines);
        XHairStyle.thickness = 2;
        XHairStyle.filled = false;

        auto gridLinesScheme = PlotStyle(tickColor, PlottingDashed, false, Nil, 1);
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
                PlotStyle{FColor::FromHex("#003f5c"), LinePrimitive::PlottingSolid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#2f4b7c"), LinePrimitive::PlottingSolid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#665191"), LinePrimitive::PlottingSolid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#a05195"), LinePrimitive::PlottingSolid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#d45087"), LinePrimitive::PlottingSolid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#f95d6a"), LinePrimitive::PlottingSolid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#ff7c43"), LinePrimitive::PlottingSolid, FILLED, Nil, 0.8},
                PlotStyle{FColor::FromHex("#ffa600"), LinePrimitive::PlottingSolid, FILLED, Nil, 0.8},
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        //XHairStyle.primitive = Dashed;
        XHairStyle.setPrimitive(Lines);
        XHairStyle.thickness = 1;
        XHairStyle.filled = false;

        auto gridLinesScheme = PlotStyle(tickColor, PlottingDotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        auto writer = std::make_shared<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(10), 24);

        return New<PlottingTheme>(PlottingTheme
                {background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, gridLinesScheme, writer,
                 writer, graphs});
    }

    GraphTheme_ptr GetSchemeElegant() {
        FColor graphTitleFont = {0,0,0,1};
        FColor graphTicksFont = {0,0,0,1};
        FColor background = {1,1,1,1};

        FColor axisColor = {0,0,0,1};
        FColor tickColor = {0,0,0,1};

        fix lineWidth = 1.2f;

        fix fill = FILLED;

        Vector<PlotStyle> graphs = {
                PlotStyle{FColor::FromHex("#ff0000"), LinePrimitive::PlottingSolid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#00ff00"), LinePrimitive::PlottingSolid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#0000ff"), LinePrimitive::PlottingSolid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#ff00ff"), LinePrimitive::PlottingSolid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#00ffff"), LinePrimitive::PlottingSolid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#ffff00"), LinePrimitive::PlottingSolid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#700000"), LinePrimitive::PlottingSolid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#007000"), LinePrimitive::PlottingSolid, fill, Nil, lineWidth},
                PlotStyle{FColor::FromHex("#000070"), LinePrimitive::PlottingSolid, fill, Nil, lineWidth},
        };

        auto XHairColor = axisColor;
        XHairColor.a = .89;
        auto XHairStyle = PlotStyle(axisColor);
        XHairStyle.setPrimitive(Lines);
        XHairStyle.thickness = 1;
        XHairStyle.filled = false;

        auto gridLinesScheme = PlotStyle(tickColor, PlottingDotDashed, false, Nil, 0.8);
        gridLinesScheme.lineColor.a = 0.15;

        // auto labelsWriter = std::make_shared<Graphics::OpenGL::WriterOpenGL>(Core::Resources::fontFileName(19), 24);
        auto ticksWriter = New<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(18), 24);
        auto& labelsWriter = ticksWriter;

        return New<PlottingTheme>(PlottingTheme{background, graphTicksFont, graphTitleFont, axisColor, tickColor, XHairStyle, gridLinesScheme, gridLinesScheme,
                         labelsWriter, ticksWriter, graphs});
    }

    GraphTheme_ptr GetSchemeGuiNativeLight()
    {
        const auto graphTitleFont = FColor::FromHex("#22272F");
        const auto graphTicksFont = FColor::FromHex("#303743");
        const auto background = FColor::FromHex("#F7F7F9");

        auto axisColor = FColor::FromHex("#3F4957");
        axisColor.a = 0.72f;
        auto tickColor = FColor::FromHex("#556275");
        tickColor.a = 0.56f;

        const auto graphs = BuildPlotStyles({
            "#2E6EA7", "#4A8F3D", "#B84E4D", "#7B62B8",
            "#C67A32", "#3F8F96", "#8D6E58", "#5D6D80"
        }, 1.55f, 0.14f);

        return BuildGuiAlignedScheme(
            graphTitleFont, graphTicksFont, background, axisColor, tickColor,
            graphs, 20, 0.09f, 1.0f, 1.0f);
    }

    GraphTheme_ptr GetSchemeGuiNativeDark()
    {
        const auto graphTitleFont = FColor::FromHex("#E4E8EF");
        const auto graphTicksFont = FColor::FromHex("#D3D9E2");
        const auto background = FColor::FromHex("#1D2026");

        auto axisColor = FColor::FromHex("#9CA8B9");
        axisColor.a = 0.47f;
        auto tickColor = FColor::FromHex("#B5BECC");
        tickColor.a = 0.40f;

        const auto graphs = BuildPlotStyles({
            "#6CA9EA", "#7ED687", "#F38F84", "#B39DDD",
            "#FFB26E", "#72D2D6", "#E6A6CE", "#9CB3CC"
        }, 1.55f, 0.20f);

        return BuildGuiAlignedScheme(
            graphTitleFont, graphTicksFont, background, axisColor, tickColor,
            graphs, 20, 0.15f, 1.0f, 1.0f);
    }

    GraphTheme_ptr GetSchemeGuiDarkRed()
    {
        const auto graphTitleFont = FColor::FromHex("#F0E1E5");
        const auto graphTicksFont = FColor::FromHex("#E1CDD3");
        const auto background = FColor::FromHex("#1E1518");

        auto axisColor = FColor::FromHex("#B08691");
        axisColor.a = 0.50f;
        auto tickColor = FColor::FromHex("#C69DA8");
        tickColor.a = 0.43f;

        const auto graphs = BuildPlotStyles({
            "#E46966", "#F0A06A", "#E6C46B", "#79C9A2",
            "#699FE0", "#AF8AE2", "#D58FB4", "#C49E88"
        }, 1.60f, 0.22f);

        return BuildGuiAlignedScheme(
            graphTitleFont, graphTicksFont, background, axisColor, tickColor,
            graphs, 20, 0.14f, 1.0f, 1.0f);
    }

    GraphTheme_ptr GetSchemeGuiStudioSlab()
    {
        const auto graphTitleFont = FColor::FromHex("#DEE3EA");
        const auto graphTicksFont = FColor::FromHex("#CDD4DD");
        const auto background = FColor::FromHex("#101419");

        auto axisColor = FColor::FromHex("#7F8B9A");
        axisColor.a = 0.52f;
        auto tickColor = FColor::FromHex("#95A1AF");
        tickColor.a = 0.43f;

        const auto graphs = BuildPlotStyles({
            "#5B8FC6", "#62AD69", "#D8676D", "#E09A47",
            "#A783C8", "#78BFC1", "#C08A72", "#91A4B8"
        }, 1.60f, 0.20f);

        return BuildGuiAlignedScheme(
            graphTitleFont, graphTicksFont, background, axisColor, tickColor,
            graphs, 20, 0.13f, 1.0f, 1.0f);
    }

    GraphTheme_ptr GetSchemeGuiScientificSlate()
    {
        const auto graphTitleFont = FColor::FromHex("#DDE7F1");
        const auto graphTicksFont = FColor::FromHex("#CEDAE6");
        const auto background = FColor::FromHex("#0D151D");

        auto axisColor = FColor::FromHex("#7C97AD");
        axisColor.a = 0.56f;
        auto tickColor = FColor::FromHex("#8EA8BE");
        tickColor.a = 0.48f;

        const auto graphs = BuildPlotStyles({
            "#58A8D7", "#4BC3C0", "#8EC56A", "#D7A56A",
            "#DF7E6F", "#AA8DD3", "#91A8BC", "#64829E"
        }, 1.62f, 0.22f);

        return BuildGuiAlignedScheme(
            graphTitleFont, graphTicksFont, background, axisColor, tickColor,
            graphs, 20, 0.16f, 1.0f, 1.0f);
    }

    GraphTheme_ptr GetSchemeGuiScientificPaper()
    {
        const auto graphTitleFont = FColor::FromHex("#2E2A23");
        const auto graphTicksFont = FColor::FromHex("#3F3930");
        const auto background = FColor::FromHex("#F4EFE4");

        auto axisColor = FColor::FromHex("#534A3E");
        axisColor.a = 0.72f;
        auto tickColor = FColor::FromHex("#6B6254");
        tickColor.a = 0.58f;

        const auto graphs = BuildPlotStyles({
            "#786446", "#A46F41", "#B16A5E", "#4F758F",
            "#6A947F", "#9273A5", "#B28A4B", "#636B77"
        }, 1.60f, 0.16f);

        auto theme = BuildGuiAlignedScheme(
            graphTitleFont, graphTicksFont, background, axisColor, tickColor,
            graphs, 21, 0.10f, 1.0f, 1.0f);
        theme->hAxisPaddingInPixels = 34;
        return theme;
    }

    GraphTheme_ptr GetSchemeGuiBlueprintNight()
    {
        const auto graphTitleFont = FColor::FromHex("#D4EEFF");
        const auto graphTicksFont = FColor::FromHex("#C4E3FA");
        const auto background = FColor::FromHex("#081727");

        auto axisColor = FColor::FromHex("#78A9CA");
        axisColor.a = 0.64f;
        auto tickColor = FColor::FromHex("#86B4D2");
        tickColor.a = 0.53f;

        const auto graphs = BuildPlotStyles({
            "#68C5FF", "#4BB9E4", "#82D5FF", "#69E0C6",
            "#9ED3FF", "#F6B472", "#F58B63", "#C6AEFF"
        }, 1.68f, 0.21f);

        return BuildGuiAlignedScheme(
            graphTitleFont, graphTicksFont, background, axisColor, tickColor,
            graphs, 20, 0.18f, 1.0f, 1.0f);
    }






} // Math
