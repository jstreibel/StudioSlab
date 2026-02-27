#ifndef STUDIOSLAB_KG_R2TOR_BASELINE_PASSIVE_MONITOR_WINDOW_V2_H
#define STUDIOSLAB_KG_R2TOR_BASELINE_PASSIVE_MONITOR_WINDOW_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Data/V2/LiveControlHubV2.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

namespace Slab::Studios::Common::Monitors::V2 {

    class FR2toRBaselinePassiveMonitorWindowV2 final : public Graphics::FWindowPanel {
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView;
        TPointer<Math::LiveData::V2::FSessionViewTopicV2> SessionTopic;
        TPointer<Math::LiveData::V2::FSessionTelemetryTopicV2> TelemetryTopic;
        TPointer<Math::LiveData::V2::FSessionStatusTopicV2> StatusTopic;
        TPointer<Math::LiveControl::V2::FLiveControlHubV2> ControlHub = nullptr;
        Str ControlTopicPrefix = "labv2/control/kg2d";
        bool bEnableControlPublisher = false;
        bool bPublishControlSource = true;
        DevFloat ControlXCenter = 0.0;
        DevFloat ControlYCenter = 0.0;
        DevFloat ControlWidth = 0.35;
        DevFloat ControlAmplitude = 0.0;
        bool bControlEnabled = false;
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
        auto PublishControlSource() -> void;
        auto DrawControlWindow() -> void;

    public:
        explicit FR2toRBaselinePassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            UIntBig maxSteps,
            const TPointer<Math::LiveControl::V2::FLiveControlHubV2> &controlHub = nullptr,
            const Str &controlTopicPrefix = "labv2/control/kg2d",
            bool bEnableControlPublisher = false,
            DevFloat controlXCenter = 0.0,
            DevFloat controlYCenter = 0.0,
            DevFloat controlWidth = 0.35,
            DevFloat controlAmplitude = 0.0,
            bool bControlEnabled = false);

        auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override;
    };

    DefinePointers(FR2toRBaselinePassiveMonitorWindowV2)

} // namespace Slab::Studios::Common::Monitors::V2

#endif // STUDIOSLAB_KG_R2TOR_BASELINE_PASSIVE_MONITOR_WINDOW_V2_H
