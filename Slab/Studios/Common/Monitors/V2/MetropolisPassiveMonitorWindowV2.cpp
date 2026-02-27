#include "MetropolisPassiveMonitorWindowV2.h"

#include "Graphics/Plot2D/PlotThemeManager.h"

#include "../../SessionLiveViewStatsV2.h"

#include <algorithm>

namespace Slab::Studios::Common::Monitors::V2 {

    auto FMetropolisPassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        GuiWindow->AddVolatileStat("Metropolis RtoR V2 passive monitor");
        GuiWindow->AddVolatileStat("");

        if (!LastSnapshot.has_value()) {
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
        GuiWindow->AddVolatileStat("Version: " + ToStr(snapshot.Version));
        GuiWindow->AddVolatileStat("Reason: " + Slab::Studios::Common::ToDisplayString(snapshot.Reason));

        if (MaxSteps > 0) {
            const auto progress = static_cast<DevFloat>(std::min<UIntBig>(snapshot.Cursor.Step, MaxSteps))
                / static_cast<DevFloat>(MaxSteps);
            GuiWindow->AddVolatileStat("Progress: " + ToStr(progress * 100.0, 2, true) + "%");
        }
    }

    auto FMetropolisPassiveMonitorWindowV2::UpdatePlotsFromSnapshot() -> void {
        if (!LastSnapshot.has_value()) return;
        const auto &snapshot = *LastSnapshot;

        if (snapshot.Phi != nullptr) {
            PhiArtist->setFunction(snapshot.Phi);
        } else {
            PhiArtist->setFunction(nullptr);
        }

        if (snapshot.Pi != nullptr) {
            PiArtist->setFunction(snapshot.Pi);
        } else {
            PiArtist->setFunction(nullptr);
        }
    }

    FMetropolisPassiveMonitorWindowV2::FMetropolisPassiveMonitorWindowV2(
            const TPointer<FMetropolisFieldSnapshotListenerV2> &snapshotListener,
            const UIntBig maxSteps)
    : FWindowPanel(Graphics::FSlabWindowConfig("Metropolis RtoR V2 GL Monitor"))
    , SnapshotListener(snapshotListener)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("Metropolis Telemetry")))
    , PhiWindow("Metropolis phi(x)")
    , PiWindow("Metropolis pi(x)")
    , MaxSteps(maxSteps) {
        if (SnapshotListener == nullptr) throw Exception("Metropolis passive monitor requires snapshot listener.");

        auto style0 = *Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0].clone();
        auto style1 = *Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[1].clone();

        PhiArtist = New<Graphics::RtoRFunctionArtist>(nullptr, style0, 2048);
        PiArtist = New<Graphics::RtoRFunctionArtist>(nullptr, style1, 2048);
        PhiArtist->SetAffectGraphRanges(true);
        PiArtist->SetAffectGraphRanges(true);

        AddWindow(GuiWindow, false, 0.28f);
        AddWindow(Naked(PhiWindow), true, 0.36f);
        AddWindow(Naked(PiWindow), true, 0.36f);
        SetColumnRelativeWidth(0, 0.28f);

        PhiWindow.AddArtist(PhiArtist);
        PiWindow.AddArtist(PiArtist);
        PhiWindow.SetAutoReviewGraphRanges(true);
        PiWindow.SetAutoReviewGraphRanges(true);
        PhiWindow.SetNoGUI();
        PiWindow.SetNoGUI();
    }

    auto FMetropolisPassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        if (SnapshotListener != nullptr) {
            const auto snapshot = SnapshotListener->TryGetSnapshot();
            if (snapshot.has_value()) LastSnapshot = snapshot;
        }

        UpdateStatsWindow();
        UpdatePlotsFromSnapshot();
        FWindowPanel::ImmediateDraw(platformWindow);
    }

} // namespace Slab::Studios::Common::Monitors::V2
