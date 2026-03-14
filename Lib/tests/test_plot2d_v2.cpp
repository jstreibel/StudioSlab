#include <catch2/catch_all.hpp>
#include <cmath>

#include "Graphics/Plot2D/V2/Plot2DV2.h"
#include "Graphics/Utils.h"

#include "Core/Model/V2/ModelSeedsV2.h"
#include "Core/Reflection/V2/ReflectionCodecsV2.h"

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

    auto CountPolylineCommands(const FPlotDrawListV2 &drawList) -> int {
        int count = 0;
        for (const auto &command : drawList.GetCommands()) {
            if (std::holds_alternative<FPolylineCommandV2>(command)) {
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

    const auto fitResult = catalog.Invoke(
        windowInterfaceId,
        "command.window.fit_to_artists",
        {},
        context);

    REQUIRE(fitResult.IsOk());
    REQUIRE(fitResult.OutputMap.at("fitted").Encoded == "true");

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
}
