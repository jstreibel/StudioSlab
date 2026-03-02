#include "IsingMetropolisPassiveMonitorWindowV2.h"

#include "Models/Ising/V2/Ising-Metropolis-RecipeV2.h"

#include "../../SessionLiveViewStatsV2.h"

#include <cmath>

namespace Slab::Studios::Common::Monitors::V2 {

    auto FIsingMetropolisPassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        std::optional<Str> extraLine = Str("Display mode: copied spin field");
        const bool bHasBoundSession = LastStatus.has_value()
            ? LastStatus->bHasBoundSession
            : (SessionTopic != nullptr && SessionTopic->HasBoundSession());
        if (LastStatus.has_value()) {
            extraLine = *extraLine + " | Run: " + Slab::Studios::Common::ToDisplayString(LastStatus->RunState);
        }

        Slab::Studios::Common::AppendSessionLiveViewStats(
            GuiWindow,
            "Ising Metropolis V2 passive monitor",
            LastTelemetry,
            bHasBoundSession,
            bLastLeaseAcquired,
            MaxSteps,
            extraLine);

        GuiWindow->AddVolatileStat("Energy density: " + ToStr(LastEnergyDensity, 6, true));
        GuiWindow->AddVolatileStat("Magnetization: " + ToStr(LastMagnetization, 6, true));
        GuiWindow->AddVolatileStat("Abs magnetization: " + ToStr(std::fabs(LastMagnetization), 6, true));
        GuiWindow->AddVolatileStat("Acceptance: " + ToStr(LastAcceptance, 6, true));
        GuiWindow->AddVolatileStat("Accepted/Rejected: " + ToStr(LastAccepted) + " / " + ToStr(LastRejected));
    }

    auto FIsingMetropolisPassiveMonitorWindowV2::SetPlotFromState(
            const TPointer<const Math::Base::EquationState> &state) -> void {
        if (LatticeArtist == nullptr) return;

        auto isingState = std::dynamic_pointer_cast<const Slab::Models::Ising::V2::FIsingLatticeStateV2>(state);
        if (isingState == nullptr) {
            LatticeArtist->setFunction(nullptr);
            return;
        }

        auto spinField = isingState->GetSpinField();
        if (spinField == nullptr) {
            LatticeArtist->setFunction(nullptr);
            return;
        }

        LastEnergyDensity = isingState->GetEnergyDensity();
        LastMagnetization = isingState->GetMagnetization();
        LastAcceptance = isingState->GetAcceptanceRatio();
        LastAccepted = isingState->GetAcceptedLastSweep();
        LastRejected = isingState->GetRejectedLastSweep();

        const auto domain = spinField->getDomain();
        const bool bGeometryChanged =
            DisplaySpins == nullptr ||
            DisplaySpins->getN() != spinField->getN() ||
            DisplaySpins->getM() != spinField->getM() ||
            !(DisplaySpins->getDomain() == domain);

        if (bGeometryChanged) {
            const auto xRes = spinField->getN();
            const auto yRes = spinField->getM();
            const auto hx = domain.getLx() / static_cast<DevFloat>(xRes);
            const auto hy = domain.getLy() / static_cast<DevFloat>(yRes);
            DisplaySpins = New<Math::R2toR::NumericFunction_CPU>(xRes, yRes, domain.xMin, domain.yMin, hx, hy);
            LatticeArtist->setFunction(DisplaySpins);
            LatticeWindow.GetRegion().setLimits(domain.xMin, domain.xMax, domain.yMin, domain.yMax);
            bPlotRegionInitialized = true;
        }

        spinField->getSpace().syncHost();
        DisplaySpins->getSpace().setToValue(spinField->getSpace());
        LatticeArtist->updateMinMax(true);

        if (!bPlotRegionInitialized) {
            LatticeWindow.GetRegion().setLimits(domain.xMin, domain.xMax, domain.yMin, domain.yMax);
            bPlotRegionInitialized = true;
        }
    }

    FIsingMetropolisPassiveMonitorWindowV2::FIsingMetropolisPassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const TPointer<Math::Numerics::V2::FStateSnapshotListenerV2> &snapshotListener,
            const UIntBig maxSteps)
    : FWindowPanel(Graphics::FSlabWindowConfig("Ising Metropolis V2 GL Monitor"))
    , LiveView(liveView)
    , SessionTopic(liveView != nullptr ? liveView->GetSessionTopic() : nullptr)
    , TelemetryTopic(liveView != nullptr ? liveView->GetTelemetryTopic() : nullptr)
    , StatusTopic(liveView != nullptr ? liveView->GetStatusTopic() : nullptr)
    , SnapshotListener(snapshotListener)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Ising Metropolis Telemetry")))
    , LatticeWindow("Ising spins(x,y)")
    , LatticeArtist(New<Graphics::R2toRFunctionArtist>())
    , MaxSteps(maxSteps) {
        if (LiveView == nullptr) throw Exception("Ising passive monitor requires a live view.");
        if (SnapshotListener == nullptr) throw Exception("Ising passive monitor requires a snapshot listener.");
        if (SessionTopic == nullptr || TelemetryTopic == nullptr) {
            throw Exception("Ising passive monitor requires session/telemetry topics.");
        }

        LatticeArtist->SetAffectGraphRanges(false);
        LatticeArtist->setDataMutable(true);

        AddWindow(Naked(LatticeWindow));

        LatticeWindow.AddArtist(LatticeArtist);
        LatticeWindow.SetAutoReviewGraphRanges(false);
        LatticeWindow.GetRegion().setLimits(-1.0, 1.0, -1.0, 1.0);
    }

    auto FIsingMetropolisPassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        if (const auto telemetry = TelemetryTopic->TryGetTelemetry(); telemetry.has_value()) {
            LastTelemetry = telemetry;
        }
        if (StatusTopic != nullptr) {
            if (const auto status = StatusTopic->TryGetStatus(); status.has_value()) {
                LastStatus = status;
            }
        }

        const auto snapshotOpt = SnapshotListener != nullptr
            ? SnapshotListener->TryGetSnapshot()
            : std::optional<Math::Numerics::V2::FStateSnapshotEnvelopeV2>{};
        bLastLeaseAcquired = snapshotOpt.has_value();
        if (snapshotOpt.has_value() &&
            (!LastSnapshotVersion.has_value() || *LastSnapshotVersion != snapshotOpt->PublishedVersion)) {
            LastSnapshotVersion = snapshotOpt->PublishedVersion;
            SetPlotFromState(snapshotOpt->State);
        }

        UpdateStatsWindow();
        FWindowPanel::ImmediateDraw(platformWindow);
    }

} // namespace Slab::Studios::Common::Monitors::V2
