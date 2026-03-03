#include "XYMetropolisPassiveMonitorWindowV2.h"

#include "Models/XY/V2/XY-Metropolis-RecipeV2.h"

#include "../../SessionLiveViewStatsV2.h"

namespace Slab::Studios::Common::Monitors::V2 {

    auto FXYMetropolisPassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        std::optional<Str> extraLine = Str("Display mode: copied theta (radians)");
        const bool bHasBoundSession = LastStatus.has_value()
            ? LastStatus->bHasBoundSession
            : (SessionTopic != nullptr && SessionTopic->HasBoundSession());
        if (LastStatus.has_value()) {
            extraLine = *extraLine + " | Run: " + Slab::Studios::Common::ToDisplayString(LastStatus->RunState);
        }

        Slab::Studios::Common::AppendSessionLiveViewStats(
            GuiWindow,
            "XY Metropolis passive monitor",
            LastTelemetry,
            bHasBoundSession,
            bLastSnapshotAvailable,
            MaxSteps,
            extraLine);

        GuiWindow->AddVolatileStat("Energy density: " + ToStr(LastEnergyDensity, 6, true));
        GuiWindow->AddVolatileStat("Magnetization: " + ToStr(LastMagnetization, 6, true));
        GuiWindow->AddVolatileStat("Acceptance: " + ToStr(LastAcceptance, 6, true));
        GuiWindow->AddVolatileStat("Accepted/Rejected: " + ToStr(LastAccepted) + " / " + ToStr(LastRejected));
    }

    auto FXYMetropolisPassiveMonitorWindowV2::SetPlotFromState(
            const TPointer<const Math::Base::EquationState> &state) -> void {
        if (LatticeArtist == nullptr) return;

        auto xyState = std::dynamic_pointer_cast<const Slab::Models::XY::V2::FXYLatticeStateV2>(state);
        if (xyState == nullptr) {
            LatticeArtist->setFunction(nullptr);
            return;
        }

        auto theta = xyState->GetThetaField();
        if (theta == nullptr) {
            LatticeArtist->setFunction(nullptr);
            return;
        }

        LastEnergyDensity = xyState->GetEnergyDensity();
        LastMagnetization = xyState->GetMagnetization();
        LastAcceptance = xyState->GetAcceptanceRatio();
        LastAccepted = xyState->GetAcceptedLastSweep();
        LastRejected = xyState->GetRejectedLastSweep();

        const auto domain = theta->getDomain();
        const bool bGeometryChanged =
            DisplayTheta == nullptr ||
            DisplayTheta->getN() != theta->getN() ||
            DisplayTheta->getM() != theta->getM() ||
            !(DisplayTheta->getDomain() == domain);

        if (bGeometryChanged) {
            const auto xRes = theta->getN();
            const auto yRes = theta->getM();
            const auto hx = domain.getLx() / static_cast<DevFloat>(xRes);
            const auto hy = domain.getLy() / static_cast<DevFloat>(yRes);
            DisplayTheta = New<Math::R2toR::NumericFunction_CPU>(xRes, yRes, domain.xMin, domain.yMin, hx, hy);
            LatticeArtist->setFunction(DisplayTheta);
            LatticeWindow.GetRegion().setLimits(domain.xMin, domain.xMax, domain.yMin, domain.yMax);
            bPlotRegionInitialized = true;
        }

        theta->getSpace().syncHost();
        DisplayTheta->getSpace().setToValue(theta->getSpace());
        LatticeArtist->updateMinMax(true);

        if (!bPlotRegionInitialized) {
            LatticeWindow.GetRegion().setLimits(domain.xMin, domain.xMax, domain.yMin, domain.yMax);
            bPlotRegionInitialized = true;
        }
    }

    FXYMetropolisPassiveMonitorWindowV2::FXYMetropolisPassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const UIntBig maxSteps)
    : FWindowPanel(Graphics::FSlabWindowConfig("XY Metropolis GL Monitor"))
    , LiveView(liveView)
    , SessionTopic(liveView != nullptr ? liveView->GetSessionTopic() : nullptr)
    , TelemetryTopic(liveView != nullptr ? liveView->GetTelemetryTopic() : nullptr)
    , StatusTopic(liveView != nullptr ? liveView->GetStatusTopic() : nullptr)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("XY Metropolis Telemetry")))
    , LatticeWindow("XY theta(x,y)")
    , LatticeArtist(New<Graphics::R2toRFunctionArtist>())
    , MaxSteps(maxSteps) {
        if (LiveView == nullptr) throw Exception("XY passive monitor requires a live view.");
        if (SessionTopic == nullptr || TelemetryTopic == nullptr || LiveView->GetSnapshotTopic() == nullptr) {
            throw Exception("XY passive monitor requires session/telemetry/snapshot topics.");
        }

        LatticeArtist->SetAffectGraphRanges(false);
        LatticeArtist->setDataMutable(true);

        AddWindow(Naked(LatticeWindow));

        LatticeWindow.AddArtist(LatticeArtist);
        LatticeWindow.SetAutoReviewGraphRanges(false);
        LatticeWindow.GetRegion().setLimits(-1.0, 1.0, -1.0, 1.0);
        LiveView->RegisterSnapshotConsumer();
    }

    FXYMetropolisPassiveMonitorWindowV2::~FXYMetropolisPassiveMonitorWindowV2() {
        if (LiveView != nullptr) LiveView->UnregisterSnapshotConsumer();
    }

    auto FXYMetropolisPassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        if (const auto telemetry = TelemetryTopic->TryGetTelemetry(); telemetry.has_value()) {
            LastTelemetry = telemetry;
        }
        if (StatusTopic != nullptr) {
            if (const auto status = StatusTopic->TryGetStatus(); status.has_value()) {
                LastStatus = status;
            }
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
