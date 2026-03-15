#include <catch2/catch_all.hpp>
#include <cmath>

#include "Graphics/Plot2D/V2/Plot2DV2.h"
#include "Graphics/Utils.h"

#include "Core/Model/V2/ModelSeedsV2.h"
#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Math/Function/RtoR2/StraightLine.h"

namespace {

    using namespace Slab;
    using namespace Slab::Graphics::Plot2D::V2;
    namespace ModelV2 = Slab::Core::Model::V2;
    using Slab::Graphics::FPoint2D;
    using Slab::Graphics::PlotStyle;

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    class FSin2XFunction final : public Slab::Math::RtoR::Function {
    public:
        auto operator()(const DevFloat x) const -> DevFloat override {
            return std::sin(2.0 * x);
        }

        auto Clone() const -> TPointer<Type> override {
            return New<FSin2XFunction>(*this);
        }
    };

    class FPlaneFunction final : public Slab::Math::R2toR::Function {
    public:
        auto operator()(Slab::Math::Real2D x) const -> DevFloat override {
            return x.x + 2.0 * x.y;
        }
    };

    auto CountPolylineCommands(const FPlotDrawListV2 &drawList) -> int {
        int count = 0;
        for (const auto &command : drawList.GetCommands()) {
            if (std::holds_alternative<FPolylineCommandV2>(command)) {
                ++count;
            }
        }

        return count;
    }

    auto CountPointSetCommands(const FPlotDrawListV2 &drawList) -> int {
        int count = 0;
        for (const auto &command : drawList.GetCommands()) {
            if (std::holds_alternative<FPointSetCommandV2>(command)) {
                ++count;
            }
        }

        return count;
    }

    auto CountRectangleCommands(const FPlotDrawListV2 &drawList) -> int {
        int count = 0;
        for (const auto &command : drawList.GetCommands()) {
            if (std::holds_alternative<FRectangleCommandV2>(command)) {
                ++count;
            }
        }

        return count;
    }

    auto CountTextCommands(const FPlotDrawListV2 &drawList) -> int {
        int count = 0;
        for (const auto &command : drawList.GetCommands()) {
            if (std::holds_alternative<FTextCommandV2>(command)) {
                ++count;
            }
        }

        return count;
    }

} // namespace

TEST_CASE("Plot2D V2 emits draw commands through backend abstraction", "[Plot2DV2]") {
    FPlot2DWindowV2 window("Render Test", {-2.0, 2.0, -2.0, 2.0}, {0, 800, 0, 600});

    auto axis = New<FAxisArtistV2>();
    auto points = New<FPointSetArtistV2>(Vector<FPoint2D>{{-1.0, -0.5}, {0.0, 1.0}, {1.5, 0.25}});
    points->SetLabel("samples");

    auto function = New<FRtoRFunctionArtistV2>(
        New<FSin2XFunction>(),
        PlotStyle(Slab::Graphics::FlatBlue, Slab::Graphics::LineStrip, false, Slab::Graphics::Nil, 1.5f),
        128,
        -2.0,
        2.0);
    function->SetLabel("sin(2x)");

    window.AddArtist(axis, -10);
    window.AddArtist(points, 0);
    window.AddArtist(function, 1);

    FRecordingRenderBackendV2 backend;
    const auto rendered = window.Render(backend);

    REQUIRE(rendered);
    REQUIRE(backend.GetRenderCallCount() == 1);

    const auto &drawList = backend.GetLastDrawList();
    REQUIRE(drawList.GetCommands().size() >= 3);
    REQUIRE(CountPolylineCommands(drawList) >= 2);
    REQUIRE(drawList.GetLegendEntries().size() == 2);

    const auto &frame = backend.GetLastFrame();
    REQUIRE(frame.WindowId == window.GetWindowId());
    REQUIRE(frame.Title == window.GetTitle());
    REQUIRE(frame.TextMetrics.FontHeightPixels > 0.0);
    REQUIRE(frame.TextMetrics.LineAdvancePixels >= frame.TextMetrics.FontHeightPixels);
    REQUIRE(frame.TextMetrics.ApproxCharacterAdvancePixels > 0.0);
    REQUIRE(frame.HudLayout.TopLeft.x == Catch::Approx(14.0));
    REQUIRE(frame.HudLayout.TopLeft.y == Catch::Approx(586.0));
    REQUIRE(frame.HudLayout.TopRight.x == Catch::Approx(786.0));
    REQUIRE(frame.HudLayout.BottomLeft.y == Catch::Approx(14.0));
}

TEST_CASE("Plot2D V2 accepts host-provided HUD layout in frame context", "[Plot2DV2]") {
    FPlot2DWindowV2 window("HUD Layout Test", {-1.0, 1.0, -1.0, 1.0}, {0, 640, 0, 480});
    window.AddArtist(New<FAxisArtistV2>(), -10);

    auto frame = window.BuildFrameContext();
    frame.HudLayout.SafeRect = {24.0, 520.0, 18.0, 450.0};
    frame.HudLayout.TopLeft = {96.0, 450.0};
    frame.HudLayout.TopRight = {520.0, 450.0};
    frame.HudLayout.BottomLeft = {24.0, 18.0};
    frame.HudLayout.BottomRight = {520.0, 18.0};
    frame.TextMetrics.FontHeightPixels = 23.0;
    frame.TextMetrics.LineAdvancePixels = 27.0;
    frame.TextMetrics.ApproxCharacterAdvancePixels = 13.0;

    FRecordingRenderBackendV2 backend;
    REQUIRE(window.Render(backend, frame));

    const auto &recorded = backend.GetLastFrame();
    REQUIRE(recorded.HudLayout.SafeRect.xMax == Catch::Approx(520.0));
    REQUIRE(recorded.HudLayout.TopLeft.x == Catch::Approx(96.0));
    REQUIRE(recorded.HudLayout.TopRight.y == Catch::Approx(450.0));
    REQUIRE(recorded.TextMetrics.FontHeightPixels == Catch::Approx(23.0));
    REQUIRE(recorded.TextMetrics.LineAdvancePixels == Catch::Approx(27.0));
    REQUIRE(recorded.TextMetrics.ApproxCharacterAdvancePixels == Catch::Approx(13.0));
}

TEST_CASE("Plot2D V2 fits region to artist bounds", "[Plot2DV2]") {
    FPlot2DWindowV2 window("Bounds Test", {-1.0, 1.0, -1.0, 1.0});

    auto points = New<FPointSetArtistV2>(Vector<FPoint2D>{{-3.0, 7.5}, {4.0, -2.0}, {6.0, 1.0}});
    points->SetAffectGraphRanges(true);
    window.AddArtist(points);

    const bool fitted = window.FitRegionToArtists(0.0);

    REQUIRE(fitted);
    REQUIRE(window.GetRegion().xMin == Catch::Approx(-3.0));
    REQUIRE(window.GetRegion().xMax == Catch::Approx(6.0));
    REQUIRE(window.GetRegion().yMin == Catch::Approx(-2.0));
    REQUIRE(window.GetRegion().yMax == Catch::Approx(7.5));
}

TEST_CASE("Plot2D V2 reflection catalog supports query and set", "[Plot2DV2][ReflectionV2]") {
    FPlot2DWindowV2 window("Reflection Test", {-1.0, 1.0, -1.0, 1.0});

    auto points = New<FPointSetArtistV2>(Vector<FPoint2D>{{-2.0, -1.0}, {2.0, 3.0}});
    points->SetLabel("reflect-points");
    window.AddArtist(points);

    auto function = New<FRtoRFunctionArtistV2>(
        New<FSin2XFunction>(),
        PlotStyle(Slab::Graphics::FlatBlue, Slab::Graphics::LineStrip, false, Slab::Graphics::Nil, 1.3f),
        16,
        -2.0,
        2.0);
    function->SetLabel("reflect-function");
    window.AddArtist(function);

    FPlotReflectionCatalogV2 catalog;
    catalog.RefreshFromLiveWindows();

    const auto windowInterfaceId = Str("v2.plot.window.") + window.GetWindowId();
    const auto artistInterfaceId = Str("v2.plot.artist.") + window.GetWindowId() + "." + points->GetArtistId();
    const auto functionArtistInterfaceId = Str("v2.plot.artist.") + window.GetWindowId() + "." + function->GetArtistId();

    REQUIRE(catalog.GetInterface(windowInterfaceId) != nullptr);
    REQUIRE(catalog.GetInterface(artistInterfaceId) != nullptr);
    REQUIRE(catalog.GetInterface(functionArtistInterfaceId) != nullptr);

    const ReflectionV2::FInvocationContextV2 context{
        .CurrentThread = ReflectionV2::EThreadAffinity::Any,
        .bRuntimeRunning = false
    };

    ReflectionV2::FValueMapV2 getInputs;
    getInputs["parameter_id"] = ReflectionV2::MakeStringValue("x_min");

    const auto getBefore = catalog.Invoke(
        windowInterfaceId,
        CPlotOperationIdQueryGetParameterV2,
        getInputs,
        context);

    REQUIRE(getBefore.IsOk());
    REQUIRE(std::stod(getBefore.OutputMap.at("value").Encoded) == Catch::Approx(-1.0));

    ReflectionV2::FValueMapV2 setInputs;
    setInputs["parameter_id"] = ReflectionV2::MakeStringValue("x_min");
    setInputs["value"] = ReflectionV2::MakeEncodedValue(ReflectionV2::CTypeIdScalarFloat64, "-3.5");

    const auto setResult = catalog.Invoke(
        windowInterfaceId,
        CPlotOperationIdCommandSetParameterV2,
        setInputs,
        context);

    REQUIRE(setResult.IsOk());
    REQUIRE(window.GetRegion().xMin == Catch::Approx(-3.5));

    ReflectionV2::FValueMapV2 setArtistVisibleInputs;
    setArtistVisibleInputs["parameter_id"] = ReflectionV2::MakeStringValue("visible");
    setArtistVisibleInputs["value"] = ReflectionV2::MakeEncodedValue(ReflectionV2::CTypeIdScalarBool, "false");

    const auto setArtistVisibleResult = catalog.Invoke(
        artistInterfaceId,
        CPlotOperationIdCommandSetParameterV2,
        setArtistVisibleInputs,
        context);

    REQUIRE(setArtistVisibleResult.IsOk());
    REQUIRE_FALSE(points->IsVisible());

    ReflectionV2::FValueMapV2 setArtistZOrderInputs;
    setArtistZOrderInputs["parameter_id"] = ReflectionV2::MakeStringValue("z_order");
    setArtistZOrderInputs["value"] = ReflectionV2::MakeEncodedValue(ReflectionV2::CTypeIdScalarInt32, "7");

    const auto setArtistZOrderResult = catalog.Invoke(
        artistInterfaceId,
        CPlotOperationIdCommandSetParameterV2,
        setArtistZOrderInputs,
        context);

    REQUIRE(setArtistZOrderResult.IsOk());
    int updatedZOrder = 0;
    REQUIRE(window.TryGetArtistZOrder(points, updatedZOrder));
    REQUIRE(updatedZOrder == 7);

    const auto fitResult = catalog.Invoke(
        windowInterfaceId,
        "command.window.fit_to_artists",
        {},
        context);

    REQUIRE(fitResult.IsOk());
    REQUIRE(fitResult.OutputMap.at("fitted").Encoded == "true");

    const auto listArtistsResult = catalog.Invoke(
        windowInterfaceId,
        "query.window.list_artists",
        {},
        context);
    REQUIRE(listArtistsResult.IsOk());
    REQUIRE(listArtistsResult.OutputMap.at("artist_count").Encoded == "2");
    REQUIRE(listArtistsResult.OutputMap.at("artist.0.id").Encoded == function->GetArtistId());
    REQUIRE(listArtistsResult.OutputMap.at("artist.1.id").Encoded == points->GetArtistId());
    REQUIRE(listArtistsResult.OutputMap.at("artist.1.z_order").Encoded == "7");
    REQUIRE(listArtistsResult.OutputMap.at("artist.1.visible").Encoded == "false");

    REQUIRE(function->GetSampleCount() == 16);
    const auto doubleSamplesResult = catalog.Invoke(
        functionArtistInterfaceId,
        CPlotOperationIdCommandArtistDoubleSampleCountV2,
        {},
        context);
    REQUIRE(doubleSamplesResult.IsOk());
    REQUIRE(doubleSamplesResult.OutputMap.at("sample_count").Encoded == "32");
    REQUIRE(function->GetSampleCount() == 32);

    ReflectionV2::FFunctionDescriptorV2 setFunctionDescriptor;
    setFunctionDescriptor.Family = "analytic";
    setFunctionDescriptor.Expression = "cos(x)";
    setFunctionDescriptor.DomainMin = -2.25;
    setFunctionDescriptor.DomainMax = 2.75;

    ReflectionV2::FValueMapV2 setFunctionInputs;
    setFunctionInputs["value"] = ReflectionV2::MakeEncodedValue(
        ReflectionV2::CTypeIdFunctionRtoR,
        ReflectionV2::EncodeFunctionDescriptorCLI(setFunctionDescriptor));

    const auto setFunctionResult = catalog.Invoke(
        functionArtistInterfaceId,
        CPlotOperationIdCommandArtistSetFunctionV2,
        setFunctionInputs,
        context);
    REQUIRE(setFunctionResult.IsOk());
    REQUIRE(function->GetFunction() != nullptr);
    REQUIRE((*function->GetFunction())(0.0) == Catch::Approx(1.0));
    REQUIRE(function->GetDomainXMin() == Catch::Approx(-2.25));
    REQUIRE(function->GetDomainXMax() == Catch::Approx(2.75));

    ReflectionV2::FValueMapV2 setDomainInputs;
    setDomainInputs["value"] = ReflectionV2::MakeStringValue("-4.5,4.25");

    const auto setDomainResult = catalog.Invoke(
        functionArtistInterfaceId,
        CPlotOperationIdCommandArtistSetDomainV2,
        setDomainInputs,
        context);
    REQUIRE(setDomainResult.IsOk());
    REQUIRE(function->GetDomainXMin() == Catch::Approx(-4.5));
    REQUIRE(function->GetDomainXMax() == Catch::Approx(4.25));
}

TEST_CASE("Plot2D V2 R2 section artist emits sampled section curves", "[Plot2DV2]") {
    FPlot2DWindowV2 window("R2 Section Test", {-1.0, 1.0, -1.0, 1.0});

    auto sectionArtist = New<FR2SectionArtistV2>(New<FPlaneFunction>(), 5);
    sectionArtist->SetLabel("sections");
    sectionArtist->SetAffectGraphRanges(true);
    sectionArtist->AddSection(
        Slab::Math::RtoR2::StraightLine::New({0.0, 0.0}, {1.0, 1.0}),
        PlotStyle(Slab::Graphics::FlatBlue, Slab::Graphics::LineStrip, false, Slab::Graphics::Nil, 1.5f),
        "diag");
    sectionArtist->AddSection(
        Slab::Math::RtoR2::StraightLine::New({0.0, 1.0}, {1.0, 0.0}),
        PlotStyle(Slab::Graphics::Red, Slab::Graphics::LineStrip, false, Slab::Graphics::Nil, 1.2f),
        "anti");

    window.AddArtist(sectionArtist);

    FRecordingRenderBackendV2 backend;
    REQUIRE(window.Render(backend));

    const auto &drawList = backend.GetLastDrawList();
    REQUIRE(CountPolylineCommands(drawList) == 2);
    REQUIRE(drawList.GetLegendEntries().size() == 2);
    REQUIRE(drawList.GetLegendEntries()[0].Label == "diag");
    REQUIRE(drawList.GetLegendEntries()[1].Label == "anti");

    const auto bounds = sectionArtist->ComputePlotBounds();
    REQUIRE(bounds.has_value());
    REQUIRE(bounds->xMin == Catch::Approx(0.0));
    REQUIRE(bounds->xMax == Catch::Approx(1.0));
    REQUIRE(bounds->yMin == Catch::Approx(0.0));
    REQUIRE(bounds->yMax == Catch::Approx(3.0));
}

TEST_CASE("Plot2D V2 R2 section artist exposes reflection-driven section controls", "[Plot2DV2][ReflectionV2]") {
    FPlot2DWindowV2 window("R2 Section Reflection", {-1.0, 1.0, -1.0, 1.0});

    auto sectionArtist = New<FR2SectionArtistV2>(New<FPlaneFunction>(), 5);
    sectionArtist->AddSection(
        Slab::Math::RtoR2::StraightLine::New({0.0, 0.0}, {1.0, 1.0}),
        PlotStyle(Slab::Graphics::FlatBlue, Slab::Graphics::LineStrip, false, Slab::Graphics::Nil, 1.5f),
        "diag");
    sectionArtist->AddSection(
        Slab::Math::RtoR2::StraightLine::New({0.0, 1.0}, {1.0, 0.0}),
        PlotStyle(Slab::Graphics::GrassGreen, Slab::Graphics::LineStrip, false, Slab::Graphics::Nil, 1.1f),
        "anti");
    window.AddArtist(sectionArtist);

    FPlotReflectionCatalogV2 catalog;
    catalog.RefreshFromLiveWindows();

    const auto artistInterfaceId = Str("v2.plot.artist.") + window.GetWindowId() + "." + sectionArtist->GetArtistId();
    REQUIRE(catalog.GetInterface(artistInterfaceId) != nullptr);

    const ReflectionV2::FInvocationContextV2 context{
        .CurrentThread = ReflectionV2::EThreadAffinity::Any,
        .bRuntimeRunning = false
    };

    ReflectionV2::FValueMapV2 getSampleCountInputs;
    getSampleCountInputs["parameter_id"] = ReflectionV2::MakeStringValue("sample_count");
    const auto getSampleCount = catalog.Invoke(
        artistInterfaceId,
        CPlotOperationIdQueryGetParameterV2,
        getSampleCountInputs,
        context);
    REQUIRE(getSampleCount.IsOk());
    REQUIRE(getSampleCount.OutputMap.at("value").Encoded == "5");

    ReflectionV2::FValueMapV2 setVisibilityInputs;
    setVisibilityInputs["parameter_id"] = ReflectionV2::MakeStringValue("section_1_visible");
    setVisibilityInputs["value"] = ReflectionV2::MakeEncodedValue(ReflectionV2::CTypeIdScalarBool, "false");
    const auto setVisibility = catalog.Invoke(
        artistInterfaceId,
        CPlotOperationIdCommandSetParameterV2,
        setVisibilityInputs,
        context);
    REQUIRE(setVisibility.IsOk());
    REQUIRE_FALSE(sectionArtist->GetSections()[1].bVisible);

    ReflectionV2::FValueMapV2 setThicknessInputs;
    setThicknessInputs["parameter_id"] = ReflectionV2::MakeStringValue("section_0_line_thickness");
    setThicknessInputs["value"] = ReflectionV2::MakeEncodedValue(ReflectionV2::CTypeIdScalarFloat64, "2.75");
    const auto setThickness = catalog.Invoke(
        artistInterfaceId,
        CPlotOperationIdCommandSetParameterV2,
        setThicknessInputs,
        context);
    REQUIRE(setThickness.IsOk());
    REQUIRE(sectionArtist->GetSections()[0].Style.thickness == Catch::Approx(2.75));

    ReflectionV2::FValueMapV2 setSampleCountInputs;
    setSampleCountInputs["parameter_id"] = ReflectionV2::MakeStringValue("sample_count");
    setSampleCountInputs["value"] = ReflectionV2::MakeEncodedValue(ReflectionV2::CTypeIdScalarInt32, "9");
    const auto setSampleCount = catalog.Invoke(
        artistInterfaceId,
        CPlotOperationIdCommandSetParameterV2,
        setSampleCountInputs,
        context);
    REQUIRE(setSampleCount.IsOk());
    REQUIRE(sectionArtist->GetSampleCount() == 9);

    FRecordingRenderBackendV2 backend;
    REQUIRE(window.Render(backend));
    REQUIRE(backend.GetLastDrawList().GetLegendEntries().size() == 1);
    REQUIRE(backend.GetLastDrawList().GetLegendEntries()[0].Label == "diag");
}

TEST_CASE("Plot2D V2 background and axis artists emit baseline visual commands", "[Plot2DV2]") {
    FPlot2DWindowV2 window("Visual Baseline", {-2.0, 2.0, -3.0, 3.0}, {0, 640, 0, 480});

    auto background = New<FBackgroundArtistV2>(Slab::Graphics::FColor(0.1f, 0.2f, 0.3f, 1.0f));
    background->SetUseThemeColor(false);
    auto axis = New<FAxisArtistV2>();
    axis->SetMajorTickCount(4);

    window.AddArtist(background, -100);
    window.AddArtist(axis, -10);

    FRecordingRenderBackendV2 backend;
    REQUIRE(window.Render(backend));

    const auto &drawList = backend.GetLastDrawList();
    REQUIRE(CountRectangleCommands(drawList) >= 1);
    REQUIRE(CountPolylineCommands(drawList) >= 4);
    REQUIRE(CountTextCommands(drawList) >= 2);

    bool foundRegionBackground = false;
    for (const auto &command : drawList.GetCommands()) {
        if (!std::holds_alternative<FRectangleCommandV2>(command)) continue;
        const auto &rectangle = std::get<FRectangleCommandV2>(command);
        if (!rectangle.bFilled) continue;
        if (rectangle.Rectangle == window.GetRegion()) {
            foundRegionBackground = true;
            break;
        }
    }

    REQUIRE(foundRegionBackground);
}

TEST_CASE("Plot2D V2 dispatches semantic graph hit targets to interactive artists", "[Plot2DV2][ModelGraph]") {
    auto model = ModelV2::BuildHarmonicOscillatorModelV2();
    const auto overview = ModelV2::BuildModelSemanticOverviewV2(model);
    const auto selectedRef = ModelV2::MakeDefinitionObjectRefV2("state.x");

    auto artist = New<FModelSemanticGraphArtistV2>();
    artist->SetSemanticOverview(overview, selectedRef);

    REQUIRE_FALSE(artist->GetNodes().empty());
    REQUIRE_FALSE(artist->GetEdges().empty());

    const auto selectedNodeIt = std::find_if(
        artist->GetNodes().begin(),
        artist->GetNodes().end(),
        [&](const auto &node) {
            return ModelV2::AreSemanticObjectRefsEqualV2(node.Ref, selectedRef);
        });
    REQUIRE(selectedNodeIt != artist->GetNodes().end());
    REQUIRE(selectedNodeIt->HopDistance == 0);
    REQUIRE(selectedNodeIt->Position.x == Catch::Approx(0.0));
    REQUIRE(selectedNodeIt->Position.y == Catch::Approx(0.0));

    FPlot2DWindowV2 window("Semantic Graph Test", {-12.0, 12.0, -10.0, 10.0}, {0, 800, 0, 600});
    window.AddArtist(artist);
    window.SetViewport({0, 800, 0, 600});

    bool bActivated = false;
    ModelV2::FSemanticObjectRefV2 activatedRef;
    artist->SetOnSemanticObjectActivated([&](const auto &ref) {
        bActivated = true;
        activatedRef = ref;
    });

    const auto viewportPosition = Slab::Graphics::FromSpaceToViewportCoord(
        selectedNodeIt->Position,
        window.GetRegion(),
        window.GetViewport());
    const auto plotPosition = window.ViewportToPlotCoord(viewportPosition);

    const auto hit = window.HitTestArtists(plotPosition, viewportPosition);
    REQUIRE(hit.has_value());
    REQUIRE(hit->Artist == artist);
    REQUIRE(hit->Target.TargetId == selectedNodeIt->NodeId);

    FPlotPointerEventV2 moveEvent;
    moveEvent.Kind = EPlotPointerEventKindV2::Move;
    moveEvent.PlotPosition = plotPosition;
    moveEvent.ViewportPosition = viewportPosition;
    REQUIRE(window.DispatchPointerEvent(moveEvent));

    FPlotPointerEventV2 pressEvent;
    pressEvent.Kind = EPlotPointerEventKindV2::Button;
    pressEvent.PlotPosition = plotPosition;
    pressEvent.ViewportPosition = viewportPosition;
    pressEvent.Button = Slab::Graphics::MouseButton_LEFT;
    pressEvent.ButtonState = Slab::Graphics::Press;
    REQUIRE(window.DispatchPointerEvent(pressEvent));

    FPlotPointerEventV2 releaseEvent;
    releaseEvent.Kind = EPlotPointerEventKindV2::Button;
    releaseEvent.PlotPosition = plotPosition;
    releaseEvent.ViewportPosition = viewportPosition;
    releaseEvent.Button = Slab::Graphics::MouseButton_LEFT;
    releaseEvent.ButtonState = Slab::Graphics::Release;
    REQUIRE(window.DispatchPointerEvent(releaseEvent));

    REQUIRE(bActivated);
    REQUIRE(ModelV2::AreSemanticObjectRefsEqualV2(activatedRef, selectedRef));
}

TEST_CASE("Plot2D V2 dispatches semantic graph keyboard controls to artists", "[Plot2DV2][ModelGraph]") {
    const auto overview = ModelV2::BuildModelSemanticOverviewV2(ModelV2::BuildHarmonicOscillatorModelV2());
    auto artist = New<FModelSemanticGraphArtistV2>();
    artist->SetSemanticOverview(overview, ModelV2::MakeDefinitionObjectRefV2("state.x"));

    FPlot2DWindowV2 window("Semantic Graph Keyboard Test", {-12.0, 12.0, -10.0, 10.0}, {0, 800, 0, 600});
    window.AddArtist(artist);

    REQUIRE(artist->GetNeighborhoodHops() == 2);
    REQUIRE(artist->GetShowLabels());

    REQUIRE(window.DispatchKeyboardEvent(FPlotKeyboardEventV2{
        .Key = Slab::Graphics::Key_RIGHT_BRACKET,
        .State = Slab::Graphics::Press
    }));
    CHECK(artist->GetNeighborhoodHops() == 3);

    REQUIRE(window.DispatchKeyboardEvent(FPlotKeyboardEventV2{
        .Key = Slab::Graphics::Key_l,
        .State = Slab::Graphics::Press
    }));
    CHECK_FALSE(artist->GetShowLabels());

    REQUIRE(window.DispatchKeyboardEvent(FPlotKeyboardEventV2{
        .Key = Slab::Graphics::Key_MINUS,
        .State = Slab::Graphics::Press
    }));
    CHECK(artist->GetNeighborhoodHops() == 2);
}

TEST_CASE("Plot2D V2 semantic graph stays drawable when switching seeded model overviews", "[Plot2DV2][ModelGraph]") {
    auto artist = New<FModelSemanticGraphArtistV2>();

    const auto oscillatorOverview = ModelV2::BuildModelSemanticOverviewV2(ModelV2::BuildHarmonicOscillatorModelV2());
    artist->SetSemanticOverview(oscillatorOverview, ModelV2::MakeDefinitionObjectRefV2("state.x"));
    REQUIRE_FALSE(artist->GetNodes().empty());
    REQUIRE_FALSE(artist->GetEdges().empty());

    const auto kleinGordonOverview = ModelV2::BuildModelSemanticOverviewV2(ModelV2::BuildKleinGordonModelV2());
    artist->SetSemanticOverview(kleinGordonOverview, ModelV2::MakeDefinitionObjectRefV2("field.phi"));
    REQUIRE_FALSE(artist->GetNodes().empty());
    REQUIRE_FALSE(artist->GetEdges().empty());

    FPlot2DWindowV2 window("Semantic Graph Model Switch Test", {-12.0, 12.0, -10.0, 10.0}, {0, 800, 0, 600});
    window.AddArtist(artist);
    REQUIRE(window.FitRegionToArtists(0.05));

    const auto drawList = window.BuildDrawList();
    CHECK(CountPolylineCommands(drawList) >= 1);
    CHECK(CountPointSetCommands(drawList) >= 2);
    CHECK(CountTextCommands(drawList) >= 2);
}

TEST_CASE("Plot2D V2 semantic graph emits screen-space HUD for hovered edges", "[Plot2DV2][ModelGraph]") {
    const auto overview = ModelV2::BuildModelSemanticOverviewV2(ModelV2::BuildHarmonicOscillatorModelV2());
    auto artist = New<FModelSemanticGraphArtistV2>();
    artist->SetShowLabels(false);
    artist->SetSemanticOverview(overview, ModelV2::MakeDefinitionObjectRefV2("state.x"));

    REQUIRE_FALSE(artist->GetEdges().empty());

    FPlot2DWindowV2 window("Semantic Graph HUD Test", {-12.0, 12.0, -10.0, 10.0}, {0, 800, 0, 600});
    window.AddArtist(artist);
    REQUIRE(window.FitRegionToArtists(0.05));
    window.SetViewport({0, 800, 0, 600});

    const auto &edge = artist->GetEdges().front();
    const auto sourceNodeIt = std::find_if(
        artist->GetNodes().begin(),
        artist->GetNodes().end(),
        [&](const auto &node) { return node.NodeId == edge.SourceNodeId; });
    const auto targetNodeIt = std::find_if(
        artist->GetNodes().begin(),
        artist->GetNodes().end(),
        [&](const auto &node) { return node.NodeId == edge.TargetNodeId; });
    REQUIRE(sourceNodeIt != artist->GetNodes().end());
    REQUIRE(targetNodeIt != artist->GetNodes().end());

    const FPoint2D midpoint{
        static_cast<DevFloat>(0.5 * (sourceNodeIt->Position.x + targetNodeIt->Position.x)),
        static_cast<DevFloat>(0.5 * (sourceNodeIt->Position.y + targetNodeIt->Position.y))
    };
    const auto viewportPosition = Slab::Graphics::FromSpaceToViewportCoord(
        midpoint,
        window.GetRegion(),
        window.GetViewport());

    REQUIRE(window.DispatchPointerEvent(FPlotPointerEventV2{
        .Kind = EPlotPointerEventKindV2::Move,
        .PlotPosition = midpoint,
        .ViewportPosition = viewportPosition
    }));

    const auto drawList = window.BuildDrawList();
    CHECK(CountRectangleCommands(drawList) >= 2);
    CHECK(CountTextCommands(drawList) >= 4);

    DevFloat maxHudWidth = 0.0;
    DevFloat maxHudHeight = 0.0;
    for (const auto &commandVariant : drawList.GetCommands()) {
        const auto *rectangle = std::get_if<FRectangleCommandV2>(&commandVariant);
        if (rectangle == nullptr) continue;
        if (!rectangle->bFilled) continue;
        if (rectangle->CoordinateSpace != EPlotCoordinateSpaceV2::Screen) continue;

        maxHudWidth = std::max(maxHudWidth, rectangle->Rectangle.GetWidth());
        maxHudHeight = std::max(maxHudHeight, rectangle->Rectangle.GetHeight());
    }

    CHECK(maxHudWidth >= 300.0);
    CHECK(maxHudHeight >= 80.0);
}
