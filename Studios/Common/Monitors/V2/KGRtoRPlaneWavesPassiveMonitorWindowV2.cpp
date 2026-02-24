#include "KGRtoRPlaneWavesPassiveMonitorWindowV2.h"

#include "Graphics/Plot2D/PlotThemeManager.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"

#include "../../SessionLiveViewStatsV2.h"

#include <cmath>

namespace Slab::Studios::Common::Monitors::V2 {

    auto FRtoRPlaneWavesPassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        Slab::Studios::Common::AppendSessionLiveViewStats(
            GuiWindow,
            "KGRtoR Plane Waves V2 passive monitor",
            LastTelemetry,
            LiveView->HasBoundSession(),
            bLastLeaseAcquired,
            MaxSteps,
            Str("Display mode: copied phi"));
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
    : FWindowPanel(Graphics::FSlabWindowConfig("KGRtoR Plane Waves V2 GL Monitor"))
    , LiveView(liveView)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("KGRtoR Telemetry")))
    , PhiWindow("KGRtoR phi(x)")
    , MaxSteps(maxSteps) {
        if (LiveView == nullptr) throw Exception("KGRtoR passive monitor requires a live view.");

        auto style = *Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0].clone();
        PhiArtist = New<Graphics::RtoRFunctionArtist>(nullptr, style, 2048);
        PhiArtist->SetAffectGraphRanges(false);

        AddWindow(GuiWindow, false, 0.25f);
        AddWindow(Naked(PhiWindow), true, 0.75f);
        SetColumnRelativeWidth(0, 0.25f);

        PhiWindow.AddArtist(PhiArtist);
        PhiWindow.SetAutoReviewGraphRanges(false);
        PhiWindow.GetRegion().setLimits(-1.0, 1.0, -1.0, 1.0);
    }

    auto FRtoRPlaneWavesPassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        const auto telemetry = LiveView->TryGetTelemetry();
        if (telemetry.has_value()) LastTelemetry = telemetry;

        auto leaseOpt = LiveView->AcquireReadLease();
        bLastLeaseAcquired = leaseOpt.has_value();

        if (leaseOpt.has_value()) SetPlotFromState(leaseOpt->GetState());
        // On lease miss or terminal invalidation, keep the last copied display frame.

        UpdateStatsWindow();
        FWindowPanel::ImmediateDraw(platformWindow);
    }

} // namespace Slab::Studios::Common::Monitors::V2
