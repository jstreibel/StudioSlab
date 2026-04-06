#include "MetropolisPassiveMonitorWindowV2.h"

#include "Graphics/Plot2D/PlotThemeManager.h"

#include "../../SessionLiveViewStatsV2.h"

#include <algorithm>

namespace Slab::Studios::Common::Monitors::V2 {

    auto FMetropolisPassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        GuiWindow->AddVolatileStat("Metropolis RtoR passive monitor");
        GuiWindow->AddVolatileStat("");

        if (!LastSnapshot.has_value() || !bLastSnapshotAvailable) {
            GuiWindow->AddVolatileStat("Waiting for snapshot...");
            return;
        }

        const auto &snapshot = *LastSnapshot;
        GuiWindow->AddVolatileStat("Step: " + ToStr(snapshot.Cursor.Step));
        if (snapshot.Cursor.SimulationTime.has_value()) {
            GuiWindow->AddVolatileStat("t: " + ToStr(*snapshot.Cursor.SimulationTime, 6, true));
        } else {
            GuiWindow->AddVolatileStat("t: n/a");
        }
        GuiWindow->AddVolatileStat("Version: " + ToStr(snapshot.PublishedVersion));
        GuiWindow->AddVolatileStat("Reason: " + Slab::Studios::Common::ToDisplayString(snapshot.LastReason));

        if (MaxSteps > 0) {
            const auto progress = static_cast<DevFloat>(std::min<UIntBig>(snapshot.Cursor.Step, MaxSteps))
                / static_cast<DevFloat>(MaxSteps);
            GuiWindow->AddVolatileStat("Progress: " + ToStr(progress * 100.0, 2, true) + "%");
        }
    }

    auto FMetropolisPassiveMonitorWindowV2::UpdatePlotsFromSnapshot() -> void {
        if (!LastSnapshot.has_value() || !bLastSnapshotAvailable) return;
        const auto &snapshot = *LastSnapshot;
        const auto state = std::dynamic_pointer_cast<const Models::KGRtoR::Metropolis::V2::FMetropolisFieldStateV2>(
            snapshot.State);
        if (state == nullptr) {
            PhiArtist->setFunction(nullptr);
            PiArtist->setFunction(nullptr);
            return;
        }

        if (const auto phi = state->GetPhiField(); phi != nullptr) {
            PhiArtist->setFunction(phi);
        } else {
            PhiArtist->setFunction(nullptr);
        }

        if (const auto pi = state->GetPiField(); pi != nullptr) {
            PiArtist->setFunction(pi);
        } else {
            PiArtist->setFunction(nullptr);
        }
    }

    FMetropolisPassiveMonitorWindowV2::FMetropolisPassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const UIntBig maxSteps)
    : FWindowPanel(Graphics::FSlabWindowConfig("Metropolis RtoR GL Monitor"))
    , LiveView(liveView)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Metropolis Telemetry")))
    , PhiWindow("Metropolis phi(x)")
    , PiWindow("Metropolis pi(x)")
    , MaxSteps(maxSteps) {
        if (LiveView == nullptr) throw Exception("Metropolis passive monitor requires a live view.");
        if (LiveView->GetSnapshotTopic() == nullptr) throw Exception("Metropolis passive monitor requires a snapshot topic.");

        auto style0 = *Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0].clone();
        auto style1 = *Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[1].clone();

        PhiArtist = New<Graphics::RtoRFunctionArtist>(nullptr, style0, 2048);
        PiArtist = New<Graphics::RtoRFunctionArtist>(nullptr, style1, 2048);
        PhiArtist->SetAffectGraphRanges(true);
        PiArtist->SetAffectGraphRanges(true);

        AddWindow(Naked(PhiWindow), false);
        AddWindow(Naked(PiWindow), true, 0.50f);
        SetColumnRelativeWidth(0, 0.50f);

        PhiWindow.AddArtist(PhiArtist);
        PiWindow.AddArtist(PiArtist);
        PhiWindow.SetAutoReviewGraphRanges(true);
        PiWindow.SetAutoReviewGraphRanges(true);
        LiveView->RegisterSnapshotConsumer();
    }

    FMetropolisPassiveMonitorWindowV2::~FMetropolisPassiveMonitorWindowV2() {
        if (LiveView != nullptr) LiveView->UnregisterSnapshotConsumer();
    }

    auto FMetropolisPassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        if (LiveView != nullptr) {
            const auto snapshot = LiveView->TryGetSnapshot();
            bLastSnapshotAvailable = snapshot.has_value() && snapshot->State != nullptr;
            if (snapshot.has_value() &&
                snapshot->State != nullptr &&
                (!LastSnapshotVersion.has_value() || *LastSnapshotVersion != snapshot->PublishedVersion)) {
                LastSnapshotVersion = snapshot->PublishedVersion;
                LastSnapshot = snapshot;
            }
        }

        UpdateStatsWindow();
        UpdatePlotsFromSnapshot();
        FWindowPanel::ImmediateDraw(platformWindow);
    }

} // namespace Slab::Studios::Common::Monitors::V2
