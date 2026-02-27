#ifndef STUDIOSLAB_KG_R2TOR_BASELINE_PASSIVE_MONITOR_WINDOW_V2_H
#define STUDIOSLAB_KG_R2TOR_BASELINE_PASSIVE_MONITOR_WINDOW_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

namespace Slab::Studios::Common::Monitors::V2 {

    class FR2toRBaselinePassiveMonitorWindowV2 final : public Graphics::FWindowPanel {
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView;
        TPointer<Math::LiveData::V2::FSessionViewTopicV2> SessionTopic;
        TPointer<Math::LiveData::V2::FSessionTelemetryTopicV2> TelemetryTopic;
        TPointer<Math::LiveData::V2::FSessionStatusTopicV2> StatusTopic;
        TPointer<Graphics::FGUIWindow> GuiWindow;
        Graphics::FPlot2DWindow FieldWindow;
        TPointer<Graphics::R2toRFunctionArtist> FieldArtist = nullptr;
        TPointer<Math::R2toR::NumericFunction_CPU> DisplayPhi = nullptr;
        bool bPlotRegionInitialized = false;

        std::optional<Math::LiveData::V2::FSessionTelemetryV2> LastTelemetry = std::nullopt;
        std::optional<Math::LiveData::V2::FSessionStatusV2> LastStatus = std::nullopt;
        bool bLastLeaseAcquired = false;
        UIntBig MaxSteps = 0;

        auto UpdateStatsWindow() -> void;
        auto SetPlotFromState(const TPointer<const Math::Base::EquationState> &state) -> void;

    public:
        explicit FR2toRBaselinePassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            UIntBig maxSteps);

        auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override;
    };

    DefinePointers(FR2toRBaselinePassiveMonitorWindowV2)

} // namespace Slab::Studios::Common::Monitors::V2

#endif // STUDIOSLAB_KG_R2TOR_BASELINE_PASSIVE_MONITOR_WINDOW_V2_H
