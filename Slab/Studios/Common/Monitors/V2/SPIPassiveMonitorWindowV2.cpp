#include "SPIPassiveMonitorWindowV2.h"

#include "Graphics/Plot2D/PlotThemeManager.h"

#include "Math/Function/RtoR2/StraightLine.h"

#include "Models/Stochastic-Path-Integral/SPI-State.h"

#include "../../SessionLiveViewStatsV2.h"

namespace Slab::Studios::Common::Monitors::V2 {

    auto FSPIPassiveMonitorWindowV2::EnsureSectionConfigured(const TPointer<Math::R2toR::FNumericFunction> &phi) -> void {
        if (phi == nullptr) return;
        if (!SectionArtist.getSections().empty()) return;

        const auto yMin = phi->getDomain().yMin;
        const auto yMax = phi->getDomain().yMax;

        auto line = New<Math::RtoR2::StraightLine>(Math::Real2D{0, yMin}, Math::Real2D{0, yMax}, yMin, yMax);
        auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0].clone();
        SectionArtist.addSection(line, style, "x=0 section");
    }

    auto FSPIPassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        Slab::Studios::Common::AppendSessionLiveViewStats(
            GuiWindow,
            "SPI passive monitor",
            LastTelemetry,
            LiveView->HasBoundSession(),
            bLastSnapshotAvailable,
            MaxSteps);
    }

    auto FSPIPassiveMonitorWindowV2::SetSectionFromState(
            const TPointer<const Math::Base::EquationState> &state) -> void {
        auto spiState = std::dynamic_pointer_cast<const Models::StochasticPathIntegrals::SPIState>(state);
        if (spiState == nullptr) {
            SectionArtist.setFunction(nullptr);
            return;
        }

        auto phi = spiState->getPhi();
        if (phi == nullptr) {
            SectionArtist.setFunction(nullptr);
            return;
        }

        EnsureSectionConfigured(phi);
        SectionArtist.setFunction(phi);
    }

    FSPIPassiveMonitorWindowV2::FSPIPassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const UIntBig maxSteps)
    : FWindowPanel(Graphics::FSlabWindowConfig("SPI GL Monitor"))
    , LiveView(liveView)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("SPI Telemetry")))
    , SectionWindow("SPI field section")
    , MaxSteps(maxSteps) {
        if (LiveView == nullptr) throw Exception("SPI passive monitor requires a live view.");
        if (LiveView->GetSnapshotTopic() == nullptr) throw Exception("SPI passive monitor requires a snapshot topic.");

        SectionArtist.SetAffectGraphRanges(true);
        AddWindow(Naked(SectionWindow));

        SectionWindow.AddArtist(Naked(SectionArtist));
        SectionWindow.SetAutoReviewGraphRanges(true);
        LiveView->RegisterSnapshotConsumer();
    }

    FSPIPassiveMonitorWindowV2::~FSPIPassiveMonitorWindowV2() {
        if (LiveView != nullptr) LiveView->UnregisterSnapshotConsumer();
    }

    auto FSPIPassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        auto telemetry = LiveView->TryGetTelemetry();
        if (telemetry.has_value()) LastTelemetry = telemetry;

        const auto snapshot = LiveView->TryGetSnapshot();
        bLastSnapshotAvailable = snapshot.has_value() && snapshot->State != nullptr;
        if (snapshot.has_value() &&
            snapshot->State != nullptr &&
            (!LastSnapshotVersion.has_value() || *LastSnapshotVersion != snapshot->PublishedVersion)) {
            LastSnapshotVersion = snapshot->PublishedVersion;
            SetSectionFromState(snapshot->State);
        }

        UpdateStatsWindow();
        FWindowPanel::ImmediateDraw(platformWindow);
    }

} // namespace Slab::Studios::Common::Monitors::V2
