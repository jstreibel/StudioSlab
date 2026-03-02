#include "SPISliceV2.h"

#include "../../Monitors/V2/SPIPassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../V2SimulationRunners.h"

#include "Core/Controller/Parameter/BuiltinParameters.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Listeners/StateSnapshotListenerV2.h"
#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/Stochastic-Path-Integral/SPINumericConfig.h"
#include "Models/Stochastic-Path-Integral/V2/SPI-RecipeV2.h"

#include <algorithm>

namespace Slab::Studios::Common::Simulations::V2 {

    namespace {

        auto ConfigureSPINumericConfig(const TPointer<Slab::Models::StochasticPathIntegrals::SPINumericConfig> &config,
                                       const FSPIExecutionConfig &cfg) -> void {
            using namespace Slab::Core;

            const auto iface = config->GetInterface();
            if (iface == nullptr) throw Exception("SPI config interface is null.");

            auto pL = DynamicPointerCast<RealParameter>(iface->GetParameter("length"));
            auto pT = DynamicPointerCast<RealParameter>(iface->GetParameter("time"));
            auto pN = DynamicPointerCast<IntegerParameter>(iface->GetParameter("site_count"));
            auto pDT = DynamicPointerCast<RealParameter>(iface->GetParameter("dT"));
            auto pNT = DynamicPointerCast<IntegerParameter>(iface->GetParameter("stochastic_time_steps"));

            if (pL == nullptr || pT == nullptr || pN == nullptr || pDT == nullptr || pNT == nullptr) {
                throw Exception("SPI config parameters are incomplete.");
            }

            pL->SetValue(cfg.L);
            pT->SetValue(cfg.Time);
            pN->SetValue(static_cast<int>(cfg.N));
            pDT->SetValue(cfg.Dt);
            pNT->SetValue(static_cast<int>(cfg.Steps));
        }

    } // namespace

    auto BuildSPINumericConfigV2(const FSPIExecutionConfig &cfg)
        -> TPointer<Slab::Models::StochasticPathIntegrals::SPINumericConfig> {
        auto numericConfig = New<Slab::Models::StochasticPathIntegrals::SPINumericConfig>();
        ConfigureSPINumericConfig(numericConfig, cfg);
        return numericConfig;
    }

    auto BuildSPIRecipeV2(const FSPIExecutionConfig &cfg,
                          const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        using namespace Slab::Models::StochasticPathIntegrals::V2;

        auto numericConfig = BuildSPINumericConfigV2(cfg);

        if (liveView != nullptr) {
            auto recipe = New<FSPIRecipeV2>(numericConfig, cfg.Interval, liveView);
            recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);
            return recipe;
        }

        return New<FSPIRecipeV2>(numericConfig, cfg.Interval);
    }

    auto BuildSPIPassiveMonitorWindowV2(const FSPIExecutionConfig &cfg,
                                        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
                                        const TPointer<Math::Numerics::V2::FStateSnapshotListenerV2> &snapshotListener)
        -> TPointer<Graphics::FSlabWindow> {
        if (liveView == nullptr) throw Exception("SPI passive monitor requires a live view.");
        if (snapshotListener == nullptr) throw Exception("SPI passive monitor requires a snapshot listener.");
        return New<Slab::Studios::Common::Monitors::V2::FSPIPassiveMonitorWindowV2>(
            liveView,
            snapshotListener,
            static_cast<UIntBig>(cfg.Steps));
    }

    auto RunSPIV2(const FSPIExecutionConfig &cfg) -> int {
        using namespace Slab::Math::Numerics::V2;

        if (cfg.bEnableGLMonitor) {
            auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
            auto baseRecipe = BuildSPIRecipeV2(cfg, liveView);
            auto snapshotListener = New<FStateSnapshotListenerV2>("SPI monitor snapshot listener V2");

            Vector<FSubscriptionV2> subscriptions = {{
                New<FEveryNStepsTriggerV2>(std::max<UIntBig>(UIntBig(1), cfg.MonitorInterval)),
                snapshotListener,
                EDeliveryModeV2::LatestOnly,
                true,
                true
            }};

            auto recipe = New<FAppendedSubscriptionsRecipeV2>(baseRecipe, std::move(subscriptions));
            auto monitor = BuildSPIPassiveMonitorWindowV2(cfg, liveView, snapshotListener);
            return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
                "Studios SPI V2 Monitor", recipe, monitor, static_cast<size_t>(cfg.Batch));
        }

        auto recipe = BuildSPIRecipeV2(cfg);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(cfg.Batch));
        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

} // namespace Slab::Studios::Common::Simulations::V2
