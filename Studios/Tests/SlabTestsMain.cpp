#include "CrashPad.h"

#include "Core/Controller/CommandLine/CommandLineParserDefs.h"
#include "Core/SlabCore.h"

#include "Graphics/Plot2D/Artists/R2SectionArtist.h"
#include "Graphics/Plot2D/Artists/RtoRFunctionArtist.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Window/SlabWindowManager.h"
#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/WindowContainer/WindowRow.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"
#include "Math/Function/RtoR2/StraightLine.h"
#include "Math/Numerics/V2/Runtime/SimulationSessionV2.h"

#include "Models/Stochastic-Path-Integral/SPI-State.h"

#include "StudioSlab.h"
#include "../Common/VisualHost.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <numbers>
#include <optional>
#include <thread>

namespace {

using namespace Slab;

auto ToDisplayString(const Math::Numerics::V2::EEventReasonV2 reason) -> Str {
    using EReason = Math::Numerics::V2::EEventReasonV2;

    switch (reason) {
    case EReason::Initial: return "Initial";
    case EReason::Scheduled: return "Scheduled";
    case EReason::Forced: return "Forced";
    case EReason::Final: return "Final";
    case EReason::AbortFinal: return "AbortFinal";
    }

    return "Unknown";
}

struct FVisualRunConfig {
    std::optional<UInt> MaxFrames = std::nullopt;
    std::optional<double> MaxSeconds = std::nullopt;
    Int Width = 1600;
    Int Height = 900;
};

class FSimplePanelWithGUI final : public Graphics::FWindowPanel {
    TPointer<Graphics::FGUIWindow> Gui;
    UInt Frame = 0;
    std::chrono::steady_clock::time_point Start = std::chrono::steady_clock::now();

public:
    FSimplePanelWithGUI()
    : FWindowPanel(Graphics::FSlabWindowConfig("Panel+GUI test"))
    , Gui(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("GUI Pane"))) {
        AddWindow(Gui, false, 1.0f);
        SetColumnRelativeWidth(0, 1.0f);
    }

    auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override {
        ++Frame;
        const auto elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - Start).count();

        Gui->AddVolatileStat("SlabTests: window-panel-gui");
        Gui->AddVolatileStat("");
        Gui->AddVolatileStat("Frame: " + ToStr(Frame));
        Gui->AddVolatileStat("Elapsed: " + ToStr(elapsed, 2, true) + " s");
        Gui->AddVolatileStat("Expected: visible ImGui text in left pane");

        FWindowPanel::ImmediateDraw(platformWindow);
    }
};

auto MakePlot2DBasicWindow() -> TPointer<Graphics::FPlot2DWindow> {
    auto plot = New<Graphics::FPlot2DWindow>("Plot2D basic");
    auto func = New<Math::RtoR::FSine>(1.0, 2.0);
    auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0];
    auto artist = New<Graphics::RtoRFunctionArtist>(func, style, 2048);
    artist->SetLabel("sin(2x)");
    artist->SetAffectGraphRanges(true);

    plot->AddArtist(artist);
    plot->SetAutoReviewGraphRanges(true);

    return plot;
}

class FPanelPlotAndGUIWindow final : public Graphics::FWindowPanel {
    TPointer<Graphics::FGUIWindow> Gui;
    TPointer<Graphics::FPlot2DWindow> Plot;
    TPointer<Graphics::RtoRFunctionArtist> Artist;
    UInt Frame = 0;

public:
    FPanelPlotAndGUIWindow()
    : FWindowPanel(Graphics::FSlabWindowConfig("Panel+Plot+GUI"))
    , Gui(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Telemetry")))
    , Plot(New<Graphics::FPlot2DWindow>("Plot")) {
        auto func = New<Math::RtoR::FSine>(1.0, 3.0);
        auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        Artist = New<Graphics::RtoRFunctionArtist>(func, style, 1024);
        Artist->SetLabel("sin(3x)");
        Artist->SetAffectGraphRanges(true);
        Plot->AddArtist(Artist);
        Plot->SetAutoReviewGraphRanges(true);

        AddWindow(Gui, false, 0.25f);
        AddWindow(Plot, true, 0.75f);
        SetColumnRelativeWidth(0, 0.25f);
    }

    auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override {
        ++Frame;
        Gui->AddVolatileStat("SlabTests: panel-plot-and-gui");
        Gui->AddVolatileStat("Frame: " + ToStr(Frame));
        Gui->AddVolatileStat("Expected: GUI + plot visible simultaneously");
        FWindowPanel::ImmediateDraw(platformWindow);
    }
};

class FRowHeavyLayoutWindow final : public Graphics::FWindowRow {
    TPointer<Graphics::FGUIWindow> LeftPane;
    TPointer<Graphics::FGUIWindow> MidPane;
    TPointer<Graphics::FPlot2DWindow> PlotPane;
    TPointer<Graphics::FGUIWindow> RightPane;
    TPointer<Graphics::RtoRFunctionArtist> PlotArtist;
    UInt Frame = 0;

public:
    FRowHeavyLayoutWindow()
    : FWindowRow("Row-heavy layout")
    , LeftPane(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Left")))
    , MidPane(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Mid")))
    , PlotPane(New<Graphics::FPlot2DWindow>("Plot"))
    , RightPane(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Right"))) {
        auto func = New<Math::RtoR::FSine>(1.0, 7.0);
        auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        PlotArtist = New<Graphics::RtoRFunctionArtist>(func, style, 2048);
        PlotArtist->SetLabel("sin(7x)");
        PlotArtist->SetAffectGraphRanges(true);
        PlotPane->AddArtist(PlotArtist);
        PlotPane->SetAutoReviewGraphRanges(true);

        AddWindow(LeftPane, Graphics::FWindowRow::Left, 0.18f);
        AddWindow(MidPane, Graphics::FWindowRow::Right, 0.22f);
        AddWindow(PlotPane, Graphics::FWindowRow::Right, 0.42f);
        AddWindow(RightPane, Graphics::FWindowRow::Right, -1.0f);
    }

    auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override {
        ++Frame;

        LeftPane->AddVolatileStat("SlabTests: row-heavy-layout");
        LeftPane->AddVolatileStat("Frame: " + ToStr(Frame));
        LeftPane->AddVolatileStat("Row pane frames visible");

        MidPane->AddVolatileStat("Mixed GUI + plot in row");
        MidPane->AddVolatileStat("Check symmetric horizontal gaps");

        RightPane->AddVolatileStat("Right pane");
        RightPane->AddVolatileStat("No clipped border lines");

        FWindowRow::ImmediateDraw(platformWindow);
    }
};

class FColumnHeavyPanelLayoutWindow final : public Graphics::FWindowPanel {
    TPointer<Graphics::FGUIWindow> LeftTop;
    TPointer<Graphics::FGUIWindow> LeftBottom;
    TPointer<Graphics::FPlot2DWindow> MidPlot;
    TPointer<Graphics::FGUIWindow> MidBottom;
    TPointer<Graphics::FGUIWindow> RightTop;
    TPointer<Graphics::FGUIWindow> RightBottom;
    TPointer<Graphics::RtoRFunctionArtist> MidArtist;
    UInt Frame = 0;

public:
    FColumnHeavyPanelLayoutWindow()
    : FWindowPanel(Graphics::FSlabWindowConfig("Column-heavy panel layout"))
    , LeftTop(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Left Top")))
    , LeftBottom(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Left Bottom")))
    , MidPlot(New<Graphics::FPlot2DWindow>("Mid Plot"))
    , MidBottom(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Mid Bottom")))
    , RightTop(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Right Top")))
    , RightBottom(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Right Bottom"))) {
        auto func = New<Math::RtoR::FSine>(1.0, 5.0);
        auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        MidArtist = New<Graphics::RtoRFunctionArtist>(func, style, 2048);
        MidArtist->SetLabel("sin(5x)");
        MidArtist->SetAffectGraphRanges(true);
        MidPlot->AddArtist(MidArtist);
        MidPlot->SetAutoReviewGraphRanges(true);

        AddWindow(LeftTop, false, 0.25f);
        AddWindow(LeftBottom, false);

        AddWindow(MidPlot, true, 0.50f);
        AddWindow(MidBottom, false);

        AddWindow(RightTop, true, 0.25f);
        AddWindow(RightBottom, false);

        SetColumnRelativeWidth(0, 0.25f);
        SetColumnRelativeWidth(1, 0.50f);
        SetColumnRelativeWidth(2, 0.25f);
    }

    auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override {
        ++Frame;

        LeftTop->AddVolatileStat("SlabTests: column-heavy-panel");
        LeftTop->AddVolatileStat("Frame: " + ToStr(Frame));
        LeftTop->AddVolatileStat("Expect symmetric pane insets");

        LeftBottom->AddVolatileStat("Column 1 / Row 2");
        LeftBottom->AddVolatileStat("Pane border should be visible");

        MidBottom->AddVolatileStat("Column 2 / Row 2");
        MidBottom->AddVolatileStat("Plot above + GUI here");

        RightTop->AddVolatileStat("Column 3 / Row 1");
        RightTop->AddVolatileStat("Nested content pane frame");

        RightBottom->AddVolatileStat("Column 3 / Row 2");
        RightBottom->AddVolatileStat("No clipped borders");

        FWindowPanel::ImmediateDraw(platformWindow);
    }
};

class FSPILiveViewMonitorWindowMock final : public Graphics::FWindowPanel {
    TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView;
    TPointer<Graphics::FGUIWindow> GuiWindow;
    TPointer<Graphics::FPlot2DWindow> SectionWindow;
    Graphics::R2SectionArtist SectionArtist;

    std::optional<Math::LiveData::V2::FSessionTelemetryV2> LastTelemetry = std::nullopt;
    bool bLastLeaseAcquired = false;

    auto EnsureSectionConfigured(const TPointer<Math::R2toR::FNumericFunction> &phi) -> void {
        if (phi == nullptr) return;
        if (!SectionArtist.getSections().empty()) return;

        const auto yMin = phi->getDomain().yMin;
        const auto yMax = phi->getDomain().yMax;

        auto line = New<Math::RtoR2::StraightLine>(Math::Real2D{0, yMin}, Math::Real2D{0, yMax}, yMin, yMax);
        auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0].clone();
        SectionArtist.addSection(line, style, "x=0 section");
    }

public:
    explicit FSPILiveViewMonitorWindowMock(const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
    : FWindowPanel(Graphics::FSlabWindowConfig("SPI live monitor mock"))
    , LiveView(liveView)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Telemetry")))
    , SectionWindow(New<Graphics::FPlot2DWindow>("SPI mock section")) {
        if (LiveView == nullptr) throw Exception("Live view is null.");

        SectionArtist.SetAffectGraphRanges(true);
        SectionWindow->AddArtist(Naked(SectionArtist));
        SectionWindow->SetAutoReviewGraphRanges(true);

        AddWindow(GuiWindow, false, 0.25f);
        AddWindow(SectionWindow, true, 0.75f);
        SetColumnRelativeWidth(0, 0.25f);
    }

    auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override {
        if (const auto telemetry = LiveView->TryGetTelemetry(); telemetry.has_value()) {
            LastTelemetry = telemetry;
        }

        // Visual test prioritizes visibility and coherence over best-effort cadence.
        auto leaseOpt = LiveView->AcquireReadLease();
        bLastLeaseAcquired = leaseOpt.has_value();

        if (leaseOpt.has_value()) {
            auto spiState = std::dynamic_pointer_cast<const Models::StochasticPathIntegrals::SPIState>(leaseOpt->GetState());
            if (spiState != nullptr) {
                auto phi = spiState->getPhi();
                EnsureSectionConfigured(phi);
                SectionArtist.setFunction(phi);
            } else {
                SectionArtist.setFunction(nullptr);
            }
        } else {
            SectionArtist.setFunction(nullptr);
        }

        GuiWindow->AddVolatileStat("SlabTests: spi-live-monitor-mock");
        GuiWindow->AddVolatileStat("");
        if (!LastTelemetry.has_value()) {
            GuiWindow->AddVolatileStat("Waiting for telemetry...");
        } else {
            const auto &t = *LastTelemetry;
            GuiWindow->AddVolatileStat("Step: " + ToStr(t.Cursor.Step));
            if (t.Cursor.SimulationTime.has_value()) {
                GuiWindow->AddVolatileStat("Sim time: " + ToStr(*t.Cursor.SimulationTime, 4, true));
            }
            GuiWindow->AddVolatileStat("Version: " + ToStr(t.PublishedVersion));
            GuiWindow->AddVolatileStat("Reason: " + ToDisplayString(t.LastReason));
            GuiWindow->AddVolatileStat(Str("Bound: ") + (LiveView->HasBoundSession() ? "yes" : "no"));
            GuiWindow->AddVolatileStat(Str("Lease: ") + (bLastLeaseAcquired ? "yes" : "no"));
        }

        FWindowPanel::ImmediateDraw(platformWindow);
    }
};

class FMockSPISessionV2 final : public Math::Numerics::V2::FSimulationSessionV2 {
    UInt N = 64;
    UInt M = 64;
    DevFloat Dt = 0.01;
    Math::Numerics::V2::FSimulationCursorV2 Cursor{};
    TPointer<Math::R2toR::NumericFunction_CPU> Phi;
    TPointer<Models::StochasticPathIntegrals::SPIState> State;
    DevFloat Phase = 0.0;

public:
    FMockSPISessionV2(const UInt n = 64, const UInt m = 64, const DevFloat dt = 0.01)
    : N(n)
    , M(m)
    , Dt(dt) {
        const auto xMin = DevFloat(-1.0);
        const auto yMin = DevFloat(-1.0);
        const auto hx = DevFloat(2.0) / static_cast<DevFloat>(std::max<UInt>(1, N));
        const auto hy = DevFloat(2.0) / static_cast<DevFloat>(std::max<UInt>(1, M));

        Phi = New<Math::R2toR::NumericFunction_CPU>(N, M, xMin, yMin, hx, hy);
        State = New<Models::StochasticPathIntegrals::SPIState>(Phi);

        Cursor.Step = 0;
        Cursor.SimulationTime = DevFloat(0.0);

        (void)StepUnsafe(1);
        Cursor.Step = 0;
        Cursor.SimulationTime = DevFloat(0.0);
    }

    auto InitializeForCurrentThread() -> void override {
    }

    [[nodiscard]] auto SupportsSimulationTime() const -> bool override {
        return true;
    }

protected:
    auto StepUnsafe(const UIntBig nSteps) -> bool override {
        if (nSteps == 0) return false;

        for (UIntBig i = 0; i < nSteps; ++i) {
            ++Cursor.Step;
            Cursor.SimulationTime = Cursor.SimulationTime.value_or(0.0) + Dt;
            Phase += 0.08;

            const auto twoPi = DevFloat(2.0) * std::numbers::pi_v<DevFloat>;
            const auto sx = twoPi / static_cast<DevFloat>(std::max<UInt>(1, N));
            const auto sy = twoPi / static_cast<DevFloat>(std::max<UInt>(1, M));

            for (UInt y = 0; y < M; ++y) {
                for (UInt x = 0; x < N; ++x) {
                    Phi->At(x, y) =
                        std::sin(sx * static_cast<DevFloat>(x) + Phase) *
                        std::cos(sy * static_cast<DevFloat>(y) - 0.5 * Phase);
                }
            }
        }

        return true;
    }

    [[nodiscard]] auto GetCursorUnsafe() const -> Math::Numerics::V2::FSimulationCursorV2 override {
        return Cursor;
    }

    [[nodiscard]] auto GetCurrentStateUnsafe() const -> Math::Base::EquationState_constptr override {
        return State;
    }
};

auto MakeEventFromLease(const Math::Numerics::V2::FSessionReadLeaseV2 &lease,
                        const TPointer<const Math::Numerics::V2::FSimulationSessionV2> &session,
                        const Math::Numerics::V2::EEventReasonV2 reason) -> Math::Numerics::V2::FSimulationEventV2 {
    Math::Numerics::V2::FSimulationEventV2 event;
    event.Cursor = lease.GetCursor();
    event.Reason = reason;
    event.State = lease.GetState();
    event.SessionRef = session;
    event.PublishedVersion = lease.GetPublishedVersion();
    return event;
}

auto ParseCommonVisualRunConfig(const int argc, const char **argv, const Str &programName, const Str &description)
-> std::pair<FVisualRunConfig, bool> {
    CLOptionsDescription options(programName, description);
    options.add_options()
        ("h,help", "Show this help")
        ("frames", "Auto-close after N render frames", cxxopts::value<UInt>())
        ("seconds", "Auto-close after seconds", cxxopts::value<double>())
        ("width", "Initial window width", cxxopts::value<Int>()->default_value("1600"))
        ("height", "Initial window height", cxxopts::value<Int>()->default_value("900"));

    const auto result = options.parse(argc, argv);
    if (result.count("help") > 0) {
        std::cout << options.help() << '\n';
        return {FVisualRunConfig{}, true};
    }

    FVisualRunConfig cfg;
    if (result.count("frames") > 0) cfg.MaxFrames = result["frames"].as<UInt>();
    if (result.count("seconds") > 0) cfg.MaxSeconds = result["seconds"].as<double>();
    cfg.Width = result["width"].as<Int>();
    cfg.Height = result["height"].as<Int>();
    return {cfg, false};
}

auto RunSingleWindowVisualTest(const FVisualRunConfig &cfg,
                               const std::function<TPointer<Graphics::FSlabWindow>()> &makeRootWindow,
                               const Str &title) -> int {
    auto host = Slab::Studios::Common::CreateGLFWVisualHost(title);

    auto rootWindow = makeRootWindow();
    if (rootWindow == nullptr) throw Exception("Visual test root window is null.");

    Slab::Studios::Common::AddRootSlabWindow(host, rootWindow, false);
    Slab::Studios::Common::AttachAutoCloseOnRenderBudget(
        host,
        {.MaxFrames = cfg.MaxFrames, .MaxSeconds = cfg.MaxSeconds});
    Slab::Studios::Common::RunVisualHost(host);
    return 0;
}

auto RunWindowPanelGuiTest(const int argc, const char **argv) -> int {
    const auto [cfg, wantsHelp] = ParseCommonVisualRunConfig(
        argc, argv, "SlabTests window-panel-gui", "Panel+GUI deferred render propagation test.");
    if (wantsHelp) return 0;

    return RunSingleWindowVisualTest(cfg, [] { return New<FSimplePanelWithGUI>(); }, "SlabTests: window-panel-gui");
}

auto RunPlot2DBasicTest(const int argc, const char **argv) -> int {
    const auto [cfg, wantsHelp] = ParseCommonVisualRunConfig(
        argc, argv, "SlabTests plot2d-basic", "Basic Plot2D visual test.");
    if (wantsHelp) return 0;

    return RunSingleWindowVisualTest(cfg, [] { return MakePlot2DBasicWindow(); }, "SlabTests: plot2d-basic");
}

auto RunPanelPlotAndGuiTest(const int argc, const char **argv) -> int {
    const auto [cfg, wantsHelp] = ParseCommonVisualRunConfig(
        argc, argv, "SlabTests panel-plot-and-gui", "Composite panel with GUI+plot visual test.");
    if (wantsHelp) return 0;

    return RunSingleWindowVisualTest(cfg, [] { return New<FPanelPlotAndGUIWindow>(); }, "SlabTests: panel-plot-and-gui");
}

auto RunRowHeavyLayoutTest(const int argc, const char **argv) -> int {
    const auto [cfg, wantsHelp] = ParseCommonVisualRunConfig(
        argc, argv, "SlabTests row-heavy-layout",
        "Row-heavy composite for pane framing/inset visual regression checks.");
    if (wantsHelp) return 0;

    return RunSingleWindowVisualTest(
        cfg,
        [] { return New<FRowHeavyLayoutWindow>(); },
        "SlabTests: row-heavy-layout");
}

auto RunColumnHeavyPanelTest(const int argc, const char **argv) -> int {
    const auto [cfg, wantsHelp] = ParseCommonVisualRunConfig(
        argc, argv, "SlabTests column-heavy-panel",
        "Column-heavy composite panel for pane framing/inset visual regression checks.");
    if (wantsHelp) return 0;

    return RunSingleWindowVisualTest(
        cfg,
        [] { return New<FColumnHeavyPanelLayoutWindow>(); },
        "SlabTests: column-heavy-panel");
}

auto RunSPILiveMonitorMockTest(const int argc, const char **argv) -> int {
    CLOptionsDescription options(
        "SlabTests spi-live-monitor-mock",
        "Mock V2 SPI live monitor using SessionLiveViewV2 (no real numerics task).");
    options.add_options()
        ("h,help", "Show this help")
        ("frames", "Auto-close after N render frames", cxxopts::value<UInt>())
        ("seconds", "Auto-close after seconds", cxxopts::value<double>()->default_value("8.0"))
        ("steps", "Mock updates before final event", cxxopts::value<UIntBig>()->default_value("500"))
        ("period-ms", "Mock publish period in milliseconds", cxxopts::value<int>()->default_value("16"))
        ("width", "Initial window width", cxxopts::value<Int>()->default_value("1600"))
        ("height", "Initial window height", cxxopts::value<Int>()->default_value("900"));

    const auto result = options.parse(argc, argv);
    if (result.count("help") > 0) {
        std::cout << options.help() << '\n';
        return 0;
    }

    FVisualRunConfig cfg;
    if (result.count("frames") > 0) cfg.MaxFrames = result["frames"].as<UInt>();
    if (result.count("seconds") > 0) cfg.MaxSeconds = result["seconds"].as<double>();
    cfg.Width = result["width"].as<Int>();
    cfg.Height = result["height"].as<Int>();

    const auto maxSteps = result["steps"].as<UIntBig>();
    const auto periodMs = std::max(1, result["period-ms"].as<int>());

    auto host = Slab::Studios::Common::CreateGLFWVisualHost("SlabTests: spi-live-monitor-mock");

    auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
    auto session = New<FMockSPISessionV2>();
    liveView->BindSession(session);

    Slab::Studios::Common::AddRootSlabWindow(host, New<FSPILiveViewMonitorWindowMock>(liveView), false);
    Slab::Studios::Common::AttachAutoCloseOnRenderBudget(
        host,
        {.MaxFrames = cfg.MaxFrames, .MaxSeconds = cfg.MaxSeconds});

    std::atomic_bool stop{false};
    std::thread producer([&]() {
        session->InitializeForCurrentThread();

        {
            const auto lease = session->AcquireReadLease();
            liveView->PublishEvent(MakeEventFromLease(lease, session, Math::Numerics::V2::EEventReasonV2::Initial));
        }

        for (UIntBig i = 0; i < maxSteps && !stop.load(); ++i) {
            session->Step(1);
            const auto lease = session->AcquireReadLease();
            liveView->PublishEvent(MakeEventFromLease(lease, session, Math::Numerics::V2::EEventReasonV2::Scheduled));
            std::this_thread::sleep_for(std::chrono::milliseconds(periodMs));
        }

        const auto lease = session->AcquireReadLease();
        const auto reason = stop.load()
            ? Math::Numerics::V2::EEventReasonV2::AbortFinal
            : Math::Numerics::V2::EEventReasonV2::Final;
        liveView->PublishEvent(MakeEventFromLease(lease, session, reason));
    });

    try {
        Slab::Studios::Common::RunVisualHost(host);
    } catch (...) {
        stop.store(true);
        if (producer.joinable()) producer.join();
        throw;
    }

    stop.store(true);
    if (producer.joinable()) producer.join();

    return 0;
}

using FCommandRunner = int(*)(int, const char**);

struct FCommandEntry {
    Str Name;
    Str Description;
    FCommandRunner Runner;
};

auto GetCommands() -> const Vector<FCommandEntry> & {
    static const Vector<FCommandEntry> Commands = {
        {"window-panel-gui", "Panel with FGUIWindow (deferred ImGui render path test)", &RunWindowPanelGuiTest},
        {"plot2d-basic", "Basic Plot2D window with sine artist", &RunPlot2DBasicTest},
        {"panel-plot-and-gui", "Composite panel containing GUI and Plot2D", &RunPanelPlotAndGuiTest},
        {"row-heavy-layout", "Row-heavy layout to inspect pane framing/insets", &RunRowHeavyLayoutTest},
        {"column-heavy-panel", "Column-heavy panel layout to inspect pane framing/insets", &RunColumnHeavyPanelTest},
        {"spi-live-monitor-mock", "Passive V2 SPI-like monitor with mock live publisher", &RunSPILiveMonitorMockTest},
    };
    return Commands;
}

auto FindCommand(const Str &name) -> const FCommandEntry * {
    const auto &commands = GetCommands();
    const auto it = std::find_if(commands.begin(), commands.end(), [&](const FCommandEntry &entry) {
        return entry.Name == name;
    });
    return it == commands.end() ? nullptr : &(*it);
}

auto PrintList() -> void {
    for (const auto &cmd : GetCommands()) {
        std::cout << cmd.Name << '\n';
    }
}

auto PrintRootUsage() -> void {
    std::cout
        << "SlabTests - Visual test runner\n\n"
        << "Usage:\n"
        << "  SlabTests list\n"
        << "  SlabTests <test-name> [test-options]\n"
        << "  SlabTests run <test-name> [test-options]\n\n"
        << "Examples:\n"
        << "  SlabTests panel-plot-and-gui --seconds 5\n"
        << "  SlabTests row-heavy-layout --seconds 5\n"
        << "  SlabTests column-heavy-panel --seconds 5\n"
        << "  SlabTests window-panel-gui --frames 300\n"
        << "  SlabTests spi-live-monitor-mock --seconds 8\n";
}

auto DispatchRoot(const int argc, const char **argv) -> int {
    if (argc <= 1) {
        PrintRootUsage();
        return 0;
    }

    const Str first = argv[1];
    if (first == "--help" || first == "-h" || first == "help") {
        PrintRootUsage();
        return 0;
    }

    if (first == "list") {
        if (argc > 2) throw Exception("Command 'list' takes no extra arguments.");
        PrintList();
        return 0;
    }

    if (first == "run") {
        if (argc <= 2) throw Exception("Missing test name after 'run'.");
        const auto *cmd = FindCommand(argv[2]);
        if (cmd == nullptr) throw Exception("Unknown visual test '" + Str(argv[2]) + "'.");
        return cmd->Runner(argc - 2, argv + 2);
    }

    const auto *cmd = FindCommand(first);
    if (cmd == nullptr) throw Exception("Unknown visual test '" + first + "'.");
    return cmd->Runner(argc - 1, argv + 1);
}

auto RunMain(const int argc, const char **argv) -> int {
    return DispatchRoot(argc, argv);
}

} // namespace

auto main(const int argc, const char **argv) -> int {
    return Slab::SafetyNet::jump(RunMain, argc, argv);
}
