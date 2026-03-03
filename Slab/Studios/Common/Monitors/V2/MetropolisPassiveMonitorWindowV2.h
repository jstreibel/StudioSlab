#ifndef STUDIOSLAB_METROPOLIS_PASSIVE_MONITOR_WINDOW_V2_H
#define STUDIOSLAB_METROPOLIS_PASSIVE_MONITOR_WINDOW_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Plot2D/Artists/RtoRFunctionArtist.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Models/KleinGordon/RtoR-Montecarlo/V2/Metropolis-FieldStateV2.h"

namespace Slab::Studios::Common::Monitors::V2 {

    class FMetropolisPassiveMonitorWindowV2 final : public Graphics::FWindowPanel {
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView;
        TPointer<Graphics::FGUIWindow> GuiWindow;
        Graphics::FPlot2DWindow PhiWindow;
        Graphics::FPlot2DWindow PiWindow;
        TPointer<Graphics::RtoRFunctionArtist> PhiArtist = nullptr;
        TPointer<Graphics::RtoRFunctionArtist> PiArtist = nullptr;

        std::optional<Math::LiveData::V2::FSessionSnapshotV2> LastSnapshot = std::nullopt;
        bool bLastSnapshotAvailable = false;
        std::optional<UIntBig> LastSnapshotVersion = std::nullopt;
        UIntBig MaxSteps = 0;

        auto UpdateStatsWindow() -> void;
        auto UpdatePlotsFromSnapshot() -> void;

    public:
        explicit FMetropolisPassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            UIntBig maxSteps);
        ~FMetropolisPassiveMonitorWindowV2() override;

        auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override;
    };

    DefinePointers(FMetropolisPassiveMonitorWindowV2)

} // namespace Slab::Studios::Common::Monitors::V2

#endif // STUDIOSLAB_METROPOLIS_PASSIVE_MONITOR_WINDOW_V2_H
