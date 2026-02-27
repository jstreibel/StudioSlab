#include "KGR2toRBaselinePassiveMonitorWindowV2.h"

#include "Models/KleinGordon/R2toR/EquationState.h"

#include "../../SessionLiveViewStatsV2.h"

namespace Slab::Studios::Common::Monitors::V2 {

    auto FR2toRBaselinePassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        std::optional<Str> extraLine = Str("Display mode: copied phi");
        const bool bHasBoundSession = LastStatus.has_value()
            ? LastStatus->bHasBoundSession
            : (SessionTopic != nullptr && SessionTopic->HasBoundSession());
        if (LastStatus.has_value()) {
            extraLine = *extraLine + " | Run: " + Slab::Studios::Common::ToDisplayString(LastStatus->RunState);
        }

        Slab::Studios::Common::AppendSessionLiveViewStats(
            GuiWindow,
            "KGR2toR Baseline V2 passive monitor",
            LastTelemetry,
            bHasBoundSession,
            bLastLeaseAcquired,
            MaxSteps,
            extraLine);
    }

    auto FR2toRBaselinePassiveMonitorWindowV2::SetPlotFromState(
            const TPointer<const Math::Base::EquationState> &state) -> void {
        if (FieldArtist == nullptr) return;

        auto kgState = std::dynamic_pointer_cast<const Math::R2toR::EquationState>(state);
        if (kgState == nullptr) {
            FieldArtist->setFunction(nullptr);
            return;
        }

        auto *phiBase = &kgState->getPhi();
        auto *phiNumeric = dynamic_cast<Math::R2toR::FNumericFunction *>(phiBase);
        if (phiNumeric == nullptr) {
            FieldArtist->setFunction(nullptr);
            return;
        }

        const auto domain = phiNumeric->getDomain();
        const bool bGeometryChanged =
            DisplayPhi == nullptr ||
            DisplayPhi->getN() != phiNumeric->getN() ||
            DisplayPhi->getM() != phiNumeric->getM() ||
            !(DisplayPhi->getDomain() == domain);

        if (bGeometryChanged) {
            const auto xRes = phiNumeric->getN();
            const auto yRes = phiNumeric->getM();
            const auto hx = domain.getLx() / static_cast<DevFloat>(xRes);
            const auto hy = domain.getLy() / static_cast<DevFloat>(yRes);
            DisplayPhi = New<Math::R2toR::NumericFunction_CPU>(xRes, yRes, domain.xMin, domain.yMin, hx, hy);
            FieldArtist->setFunction(DisplayPhi);
            FieldWindow.GetRegion().setLimits(domain.xMin, domain.xMax, domain.yMin, domain.yMax);
            bPlotRegionInitialized = true;
        }

        phiNumeric->getSpace().syncHost();
        DisplayPhi->getSpace().setToValue(phiNumeric->getSpace());
        FieldArtist->updateMinMax(true);

        if (!bPlotRegionInitialized) {
            FieldWindow.GetRegion().setLimits(domain.xMin, domain.xMax, domain.yMin, domain.yMax);
            bPlotRegionInitialized = true;
        }
    }

    FR2toRBaselinePassiveMonitorWindowV2::FR2toRBaselinePassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const UIntBig maxSteps)
    : FWindowPanel(Graphics::FSlabWindowConfig("KGR2toR Baseline V2 GL Monitor"))
    , LiveView(liveView)
    , SessionTopic(liveView != nullptr ? liveView->GetSessionTopic() : nullptr)
    , TelemetryTopic(liveView != nullptr ? liveView->GetTelemetryTopic() : nullptr)
    , StatusTopic(liveView != nullptr ? liveView->GetStatusTopic() : nullptr)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("KGR2toR Telemetry")))
    , FieldWindow("KGR2toR phi(x,y)")
    , FieldArtist(New<Graphics::R2toRFunctionArtist>())
    , MaxSteps(maxSteps) {
        if (LiveView == nullptr) throw Exception("KGR2toR passive monitor requires a live view.");
        if (SessionTopic == nullptr || TelemetryTopic == nullptr) {
            throw Exception("KGR2toR passive monitor requires session/telemetry topics.");
        }

        FieldArtist->SetAffectGraphRanges(false);
        FieldArtist->setDataMutable(true);

        AddWindow(GuiWindow, false, 0.30f);
        AddWindow(Naked(FieldWindow), true, 0.70f);
        SetColumnRelativeWidth(0, 0.30f);

        FieldWindow.AddArtist(FieldArtist);
        FieldWindow.SetAutoReviewGraphRanges(false);
        FieldWindow.SetNoGUI();
        FieldWindow.GetRegion().setLimits(-1.0, 1.0, -1.0, 1.0);
    }

    auto FR2toRBaselinePassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        const auto telemetry = TelemetryTopic->TryGetTelemetry();
        if (telemetry.has_value()) LastTelemetry = telemetry;
        if (StatusTopic != nullptr) {
            const auto status = StatusTopic->TryGetStatus();
            if (status.has_value()) LastStatus = status;
        }

        auto leaseOpt = SessionTopic->AcquireReadLease();
        bLastLeaseAcquired = leaseOpt.has_value();

        if (leaseOpt.has_value()) SetPlotFromState(leaseOpt->GetState());

        UpdateStatsWindow();
        FWindowPanel::ImmediateDraw(platformWindow);
    }

} // namespace Slab::Studios::Common::Monitors::V2
