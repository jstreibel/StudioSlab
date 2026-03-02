#ifndef STUDIOSLAB_SPI_PASSIVE_MONITOR_WINDOW_V2_H
#define STUDIOSLAB_SPI_PASSIVE_MONITOR_WINDOW_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Plot2D/Artists/R2SectionArtist.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Math/Numerics/V2/Listeners/StateSnapshotListenerV2.h"

namespace Slab::Studios::Common::Monitors::V2 {

    class FSPIPassiveMonitorWindowV2 final : public Graphics::FWindowPanel {
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView;
        TPointer<Math::Numerics::V2::FStateSnapshotListenerV2> SnapshotListener;
        TPointer<Graphics::FGUIWindow> GuiWindow;
        Graphics::FPlot2DWindow SectionWindow;
        Graphics::R2SectionArtist SectionArtist;

        std::optional<Math::LiveData::V2::FSessionTelemetryV2> LastTelemetry = std::nullopt;
        bool bLastLeaseAcquired = false;
        std::optional<UIntBig> LastSnapshotVersion = std::nullopt;
        UIntBig MaxSteps = 0;

        auto EnsureSectionConfigured(const TPointer<Math::R2toR::FNumericFunction> &phi) -> void;
        auto SetSectionFromState(const TPointer<const Math::Base::EquationState> &state) -> void;
        auto UpdateStatsWindow() -> void;

    public:
        explicit FSPIPassiveMonitorWindowV2(const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
                                            const TPointer<Math::Numerics::V2::FStateSnapshotListenerV2> &snapshotListener,
                                            UIntBig maxSteps);

        auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override;
    };

    DefinePointers(FSPIPassiveMonitorWindowV2)

} // namespace Slab::Studios::Common::Monitors::V2

#endif // STUDIOSLAB_SPI_PASSIVE_MONITOR_WINDOW_V2_H
