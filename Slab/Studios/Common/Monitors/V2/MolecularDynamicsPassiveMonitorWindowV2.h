#ifndef STUDIOSLAB_MOLECULAR_DYNAMICS_PASSIVE_MONITOR_WINDOW_V2_H
#define STUDIOSLAB_MOLECULAR_DYNAMICS_PASSIVE_MONITOR_WINDOW_V2_H

#include "Core/SlabCore.h"

#include "Graphics/Plot2D/Artists/PointSetArtist.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/VectorSpace/Impl/PointSet.h"

namespace Slab::Studios::Common::Monitors::V2 {

    class FMolecularDynamicsPassiveMonitorWindowV2 final : public Graphics::FWindowPanel {
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView;
        TPointer<Math::LiveData::V2::FSessionViewTopicV2> SessionTopic;
        TPointer<Math::LiveData::V2::FSessionTelemetryTopicV2> TelemetryTopic;
        TPointer<Math::LiveData::V2::FSessionStatusTopicV2> StatusTopic;

        TPointer<Graphics::FGUIWindow> GuiWindow;
        Graphics::FPlot2DWindow ParticleWindow;
        TPointer<Math::FPointSet> ParticlePoints = nullptr;
        TPointer<Graphics::PointSetArtist> ParticleArtist = nullptr;

        std::optional<Math::LiveData::V2::FSessionTelemetryV2> LastTelemetry = std::nullopt;
        std::optional<Math::LiveData::V2::FSessionStatusV2> LastStatus = std::nullopt;
        bool bLastSnapshotAvailable = false;
        std::optional<UIntBig> LastSnapshotVersion = std::nullopt;
        UIntBig MaxSteps = 0;
        DevFloat BoxLength = 50.0;
        DevFloat LastMeanRadius = 0.0;
        DevFloat LastKineticEnergy = 0.0;
        UIntBig LastParticleCount = 0;

        auto UpdateStatsWindow() -> void;
        auto UpdateParticlePointsFromState(const TPointer<const Math::Base::EquationState> &state) -> void;

    public:
        explicit FMolecularDynamicsPassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            UIntBig maxSteps,
            DevFloat boxLength);
        ~FMolecularDynamicsPassiveMonitorWindowV2() override;

        auto ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void override;
    };

    DefinePointers(FMolecularDynamicsPassiveMonitorWindowV2)

} // namespace Slab::Studios::Common::Monitors::V2

#endif // STUDIOSLAB_MOLECULAR_DYNAMICS_PASSIVE_MONITOR_WINDOW_V2_H
