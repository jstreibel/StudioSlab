#include "KGRtoRPlaneWavesPassiveMonitorWindowV2.h"

#include "Graphics/Plot2D/PlotThemeManager.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"

#include "../../SessionLiveViewStatsV2.h"

#include <cmath>

namespace Slab::Studios::Common::Monitors::V2 {

    auto FRtoRPlaneWavesPassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        std::optional<Str> extraLine = Str("Display mode: copied phi");
        const bool bHasBoundSession = LastStatus.has_value()
            ? LastStatus->bHasBoundSession
            : (SessionTopic != nullptr && SessionTopic->HasBoundSession());
        if (LastStatus.has_value()) {
            extraLine = *extraLine + " | Run: " + Slab::Studios::Common::ToDisplayString(LastStatus->RunState);
        }

        Slab::Studios::Common::AppendSessionLiveViewStats(
            GuiWindow,
            "KGRtoR Plane Waves passive monitor",
            LastTelemetry,
            bHasBoundSession,
            bLastSnapshotAvailable,
            MaxSteps,
            extraLine);
    }

    auto FRtoRPlaneWavesPassiveMonitorWindowV2::SetPlotFromState(
            const TPointer<const Math::Base::EquationState> &state) -> void {
        if (PhiArtist == nullptr) return;

        auto kgState = std::dynamic_pointer_cast<const Models::KGRtoR::FEquationState>(state);
        if (kgState == nullptr) {
            PhiArtist->setFunction(nullptr);
            return;
        }

        auto *phiBase = &kgState->getPhi();
        auto *phiNumeric = dynamic_cast<Math::RtoR::NumericFunction *>(phiBase);
        if (phiNumeric == nullptr) {
            PhiArtist->setFunction(nullptr);
            return;
        }

        const bool bGeometryChanged =
            DisplayPhi == nullptr ||
            DisplayPhi->N != phiNumeric->N ||
            DisplayPhi->xMin != phiNumeric->xMin ||
            DisplayPhi->xMax != phiNumeric->xMax;

        if (bGeometryChanged) {
            DisplayPhi = New<Math::RtoR::NumericFunction_CPU>(*phiNumeric);
        } else {
            DisplayPhi->Set(phiNumeric->getSpace().getHostData(true));
        }

        PhiArtist->setFunction(DisplayPhi);

        DevFloat yMin = 0.0;
        DevFloat yMax = 0.0;
        try {
            yMin = phiNumeric->min();
            yMax = phiNumeric->max();
        } catch (...) {
            yMin = -1.0;
            yMax = 1.0;
        }

        if (!std::isfinite(yMin) || !std::isfinite(yMax)) {
            yMin = -1.0;
            yMax = 1.0;
        }
        if (::Slab::Common::AreEqual(yMin, yMax)) {
            const auto pad = ::Slab::Common::AreEqual(yMin, 0.0) ? DevFloat(1.0) : std::abs(yMin) * DevFloat(0.1);
            yMin -= pad;
            yMax += pad;
        }

        // Do not reset the view on every frame; it prevents mouse zoom/pan.
        if (!bPlotRegionInitialized || bGeometryChanged) {
            PhiWindow.GetRegion().setLimits(phiNumeric->xMin, phiNumeric->xMax, yMin, yMax);
            bPlotRegionInitialized = true;
        }
    }

    FRtoRPlaneWavesPassiveMonitorWindowV2::FRtoRPlaneWavesPassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const UIntBig maxSteps)
    : FWindowPanel(Graphics::FSlabWindowConfig("KGRtoR Plane Waves GL Monitor"))
    , LiveView(liveView)
    , SessionTopic(liveView != nullptr ? liveView->GetSessionTopic() : nullptr)
    , TelemetryTopic(liveView != nullptr ? liveView->GetTelemetryTopic() : nullptr)
    , StatusTopic(liveView != nullptr ? liveView->GetStatusTopic() : nullptr)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("KGRtoR Telemetry")))
    , PhiWindow("KGRtoR phi(x)")
    , MaxSteps(maxSteps) {
        if (LiveView == nullptr) throw Exception("KGRtoR passive monitor requires a live view.");
        if (SessionTopic == nullptr || TelemetryTopic == nullptr || LiveView->GetSnapshotTopic() == nullptr) {
            throw Exception("KGRtoR passive monitor requires session/telemetry/snapshot topics.");
        }

        auto style = *Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0].clone();
        PhiArtist = New<Graphics::RtoRFunctionArtist>(nullptr, style, 2048);
        PhiArtist->SetAffectGraphRanges(false);

        AddWindow(Naked(PhiWindow));

        PhiWindow.AddArtist(PhiArtist);
        PhiWindow.SetAutoReviewGraphRanges(false);
        PhiWindow.GetRegion().setLimits(-1.0, 1.0, -1.0, 1.0);
        LiveView->RegisterSnapshotConsumer();
    }

    FRtoRPlaneWavesPassiveMonitorWindowV2::~FRtoRPlaneWavesPassiveMonitorWindowV2() {
        if (LiveView != nullptr) LiveView->UnregisterSnapshotConsumer();
    }

    auto FRtoRPlaneWavesPassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        const auto telemetry = TelemetryTopic->TryGetTelemetry();
        if (telemetry.has_value()) LastTelemetry = telemetry;
        if (StatusTopic != nullptr) {
            const auto status = StatusTopic->TryGetStatus();
            if (status.has_value()) LastStatus = status;
        }

        const auto snapshot = LiveView->TryGetSnapshot();
        bLastSnapshotAvailable = snapshot.has_value() && snapshot->State != nullptr;
        if (snapshot.has_value() &&
            snapshot->State != nullptr &&
            (!LastSnapshotVersion.has_value() || *LastSnapshotVersion != snapshot->PublishedVersion)) {
            LastSnapshotVersion = snapshot->PublishedVersion;
            SetPlotFromState(snapshot->State);
        }

        UpdateStatsWindow();
        FWindowPanel::ImmediateDraw(platformWindow);
    }

} // namespace Slab::Studios::Common::Monitors::V2
