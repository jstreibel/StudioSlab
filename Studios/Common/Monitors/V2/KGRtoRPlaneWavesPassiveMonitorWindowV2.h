#ifndef STUDIOSLAB_KG_RTOR_PLANEWAVES_PASSIVE_MONITOR_WINDOW_V2_H
#define STUDIOSLAB_KG_RTOR_PLANEWAVES_PASSIVE_MONITOR_WINDOW_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Plot2D/Artists/RtoRFunctionArtist.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

namespace Slab::Studios::Common::Monitors::V2 {

    class FRtoRPlaneWavesPassiveMonitorWindowV2 final : public Graphics::FWindowPanel {
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView;
        TPointer<Graphics::FGUIWindow> GuiWindow;
        Graphics::FPlot2DWindow PhiWindow;
        TPointer<Graphics::RtoRFunctionArtist> PhiArtist = nullptr;
        TPointer<Math::RtoR::NumericFunction_CPU> DisplayPhi = nullptr;
        bool bPlotRegionInitialized = false;

        std::optional<Math::LiveData::V2::FSessionTelemetryV2> LastTelemetry = std::nullopt;
        bool bLastLeaseAcquired = false;
        UIntBig MaxSteps = 0;

        auto UpdateStatsWindow() -> void;
        auto SetPlotFromState(const TPointer<const Math::Base::EquationState> &state) -> void;

    public:
        explicit FRtoRPlaneWavesPassiveMonitorWindowV2(const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
                                                       UIntBig maxSteps);

        auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override;
    };

    DefinePointers(FRtoRPlaneWavesPassiveMonitorWindowV2)

} // namespace Slab::Studios::Common::Monitors::V2

#endif // STUDIOSLAB_KG_RTOR_PLANEWAVES_PASSIVE_MONITOR_WINDOW_V2_H
