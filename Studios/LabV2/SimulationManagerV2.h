#ifndef STUDIOSLAB_LAB_V2_SIMULATION_MANAGER_V2_H
#define STUDIOSLAB_LAB_V2_SIMULATION_MANAGER_V2_H

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"
#include "Graphics/Window/SlabWindow.h"
#include "Math/Data/V2/LiveDataHubV2.h"
#include "Math/Data/V2/LiveControlHubV2.h"

#include "Studios/Common/Simulations/V2/SPISliceV2.h"
#include "Studios/Common/Simulations/V2/KGR2toRBaselineSliceV2.h"
#include "Studios/Common/Simulations/V2/KGRtoRPlaneWavesSliceV2.h"
#include "Studios/Common/Simulations/V2/MolecularDynamicsSliceV2.h"
#include "Studios/Common/Simulations/V2/MetropolisSliceV2.h"
#include "Studios/Common/Simulations/V2/XYSliceV2.h"
#include "Studios/Common/Simulations/V2/IsingSliceV2.h"

#include <functional>

class FSimulationManagerV2 final : public Slab::Graphics::FPlatformWindowEventListener {
public:
    using FAddWindowFn = std::function<void(const Slab::TPointer<Slab::Graphics::FSlabWindow> &)>;

    FSimulationManagerV2(
        Slab::TPointer<Slab::Graphics::FImGuiContext> imGuiContext,
        Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2> liveDataHub,
        Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> liveControlHub,
        FAddWindowFn addWindowFn);

    bool NotifyRender(const Slab::Graphics::FPlatformWindow &platformWindow) override;
    auto EnsureLauncherVisible() -> void;
    auto SetLauncherVisible(bool visible) -> void;
    auto RequestLauncherInitialDock(unsigned int dockId) -> void;

private:
    Slab::TPointer<Slab::Graphics::FImGuiContext> ImGuiContext;
    Slab::TPointer<Slab::Math::LiveData::V2::FLiveDataHubV2> LiveDataHub;
    Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> LiveControlHub;
    FAddWindowFn AddWindow;

    bool bShowLauncherWindow = true;
    bool bRequestLauncherInitialDock = false;
    unsigned int LauncherInitialDockId = 0;
    bool bSPIPublishLiveViewHeadless = true;
    bool bRtoRPublishLiveViewHeadless = true;
    bool bR2toRPublishLiveViewHeadless = true;
    bool bMolecularDynamicsPublishLiveViewHeadless = true;
    bool bXYPublishLiveViewHeadless = true;
    bool bIsingPublishLiveViewHeadless = true;

    Slab::Studios::Common::Simulations::V2::FSPIExecutionConfig SPICfg;
    Slab::Studios::Common::Simulations::V2::FRtoRPlaneWavesExecutionConfig RtoRCfg;
    Slab::Studios::Common::Simulations::V2::FR2toRBaselineExecutionConfig R2toRCfg;
    Slab::Studios::Common::Simulations::V2::FMolecularDynamicsExecutionConfigV2 MolecularDynamicsCfg;
    Slab::Studios::Common::Simulations::V2::FMetropolisExecutionConfigV2 MetropolisCfg;
    Slab::Studios::Common::Simulations::V2::FXYExecutionConfigV2 XYCfg;
    Slab::Studios::Common::Simulations::V2::FIsingExecutionConfigV2 IsingCfg;

    Slab::UIntBig SPIRunCounter = 0;
    Slab::UIntBig RtoRRunCounter = 0;
    Slab::UIntBig R2toRRunCounter = 0;
    Slab::UIntBig MolecularDynamicsRunCounter = 0;
    Slab::UIntBig XYRunCounter = 0;
    Slab::UIntBig IsingRunCounter = 0;

    Slab::Str LastError;

    auto AddMenus(const Slab::Graphics::FPlatformWindow &platformWindow) -> void;
    auto DrawLauncherWindow() -> void;

    auto DrawSPISection() -> void;
    auto DrawRtoRSection() -> void;
    auto DrawR2toRSection() -> void;
    auto DrawMolecularDynamicsSection() -> void;
    auto DrawMetropolisSection() -> void;
    auto DrawXYSection() -> void;
    auto DrawIsingSection() -> void;

    auto LaunchSPI(bool enableMonitor) -> void;
    auto LaunchRtoR(bool enableMonitor) -> void;
    auto LaunchR2toR(bool enableMonitor) -> void;
    auto LaunchMolecularDynamics(bool enableMonitor) -> void;
    auto LaunchMetropolis(bool enableMonitor) -> void;
    auto LaunchXY(bool enableMonitor) -> void;
    auto LaunchIsing(bool enableMonitor) -> void;

    [[nodiscard]] auto MakeTopicName(const Slab::Str &prefix, Slab::UIntBig &counter) -> Slab::Str;
    auto LaunchNumericTask(const Slab::TPointer<Slab::Math::Numerics::V2::FSimulationRecipeV2> &recipe,
                           Slab::UIntBig batch,
                           const Slab::Str &taskNameHint) -> void;
};

#endif // STUDIOSLAB_LAB_V2_SIMULATION_MANAGER_V2_H
