#include "MolecularDynamicsPassiveMonitorWindowV2.h"

#include "Graphics/Plot2D/PlotThemeManager.h"

#include "Models/MolecularDynamics/Hamiltonians/MoleculesState.h"

#include "../../SessionLiveViewStatsV2.h"

#include <algorithm>
#include <cmath>

namespace Slab::Studios::Common::Monitors::V2 {

    auto FMolecularDynamicsPassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        std::optional<Str> extraLine = Str("Display mode: particle cloud");
        const bool bHasBoundSession = LastStatus.has_value()
            ? LastStatus->bHasBoundSession
            : (SessionTopic != nullptr && SessionTopic->HasBoundSession());
        if (LastStatus.has_value()) {
            extraLine = *extraLine + " | Run: " + Slab::Studios::Common::ToDisplayString(LastStatus->RunState);
        }

        Slab::Studios::Common::AppendSessionLiveViewStats(
            GuiWindow,
            "Molecular Dynamics V2 passive monitor",
            LastTelemetry,
            bHasBoundSession,
            bLastLeaseAcquired,
            MaxSteps,
            extraLine);

        GuiWindow->AddVolatileStat("Particles: " + ToStr(LastParticleCount));
        GuiWindow->AddVolatileStat("Mean radius: " + ToStr(LastMeanRadius, 6, true));
        GuiWindow->AddVolatileStat("Kinetic: " + ToStr(LastKineticEnergy, 6, true));
    }

    auto FMolecularDynamicsPassiveMonitorWindowV2::UpdateParticlePointsFromState(
            const TPointer<const Math::Base::EquationState> &state) -> void {
        if (ParticlePoints == nullptr || ParticleArtist == nullptr) return;

        auto molecules = std::dynamic_pointer_cast<const Slab::Models::MolecularDynamics::FMoleculesState>(state);
        if (molecules == nullptr) {
            ParticlePoints->Clear();
            LastParticleCount = 0;
            LastMeanRadius = 0.0;
            LastKineticEnergy = 0.0;
            return;
        }

        const auto &q = molecules->first();
        const auto &p = molecules->second();

        ParticlePoints->Clear();
        LastParticleCount = q.size();
        LastMeanRadius = 0.0;
        LastKineticEnergy = 0.0;

        for (size_t i = 0; i < q.size(); ++i) {
            const auto &qi = q[i];
            ParticlePoints->AddPoint(qi.x, qi.y);
            LastMeanRadius += std::sqrt(qi.x * qi.x + qi.y * qi.y);

            if (i < p.size()) {
                const auto &pi = p[i];
                LastKineticEnergy += 0.5 * (pi.x * pi.x + pi.y * pi.y);
            }
        }

        if (!q.empty()) {
            LastMeanRadius /= static_cast<DevFloat>(q.size());
        }

        ParticleArtist->setPointSet(ParticlePoints);
    }

    FMolecularDynamicsPassiveMonitorWindowV2::FMolecularDynamicsPassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const UIntBig maxSteps,
            const DevFloat boxLength)
    : FWindowPanel(Graphics::FSlabWindowConfig("Molecular Dynamics V2 GL Monitor"))
    , LiveView(liveView)
    , SessionTopic(liveView != nullptr ? liveView->GetSessionTopic() : nullptr)
    , TelemetryTopic(liveView != nullptr ? liveView->GetTelemetryTopic() : nullptr)
    , StatusTopic(liveView != nullptr ? liveView->GetStatusTopic() : nullptr)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Molecular Dynamics Telemetry")))
    , ParticleWindow("Molecular Dynamics Particles")
    , ParticlePoints(New<Math::FPointSet>())
    , MaxSteps(maxSteps)
    , BoxLength(std::max<DevFloat>(boxLength, 1e-6)) {
        if (LiveView == nullptr) throw Exception("MolecularDynamics passive monitor requires a live view.");
        if (SessionTopic == nullptr || TelemetryTopic == nullptr) {
            throw Exception("MolecularDynamics passive monitor requires session/telemetry topics.");
        }

        auto style = *Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0].clone();
        style.setPrimitive(Graphics::LinePrimitive::Points);
        style.filled = false;
        style.thickness = 2.0f;

        ParticleArtist = New<Graphics::PointSetArtist>(ParticlePoints, style);
        ParticleArtist->SetAffectGraphRanges(false);
        ParticleArtist->SetLabel("particles");

        AddWindow(GuiWindow, false, 0.28f);
        AddWindow(Naked(ParticleWindow), true, 0.72f);
        SetColumnRelativeWidth(0, 0.28f);

        ParticleWindow.AddArtist(ParticleArtist);
        ParticleWindow.SetAutoReviewGraphRanges(false);
        ParticleWindow.SetNoGUI();

        const auto half = 0.5 * BoxLength;
        ParticleWindow.GetRegion().setLimits(-half, half, -half, half);
    }

    auto FMolecularDynamicsPassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        const auto telemetry = TelemetryTopic->TryGetTelemetry();
        if (telemetry.has_value()) LastTelemetry = telemetry;
        if (StatusTopic != nullptr) {
            const auto status = StatusTopic->TryGetStatus();
            if (status.has_value()) LastStatus = status;
        }

        auto leaseOpt = SessionTopic->AcquireReadLease();
        bLastLeaseAcquired = leaseOpt.has_value();
        if (leaseOpt.has_value()) {
            UpdateParticlePointsFromState(leaseOpt->GetState());
        }

        UpdateStatsWindow();
        FWindowPanel::ImmediateDraw(platformWindow);
    }

} // namespace Slab::Studios::Common::Monitors::V2
