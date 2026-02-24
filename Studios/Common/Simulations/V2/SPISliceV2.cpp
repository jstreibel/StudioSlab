#include "SPISliceV2.h"

#include "../../Monitors/V2/SPIPassiveMonitorWindowV2.h"

#include "Core/Controller/Parameter/BuiltinParameters.h"

#include "Models/Stochastic-Path-Integral/SPINumericConfig.h"
#include "Models/Stochastic-Path-Integral/V2/SPI-RecipeV2.h"

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
                                        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Graphics::FSlabWindow> {
        if (liveView == nullptr) throw Exception("SPI passive monitor requires a live view.");
        return New<Slab::Studios::Common::Monitors::V2::FSPIPassiveMonitorWindowV2>(
            liveView, static_cast<UIntBig>(cfg.Steps));
    }

} // namespace Slab::Studios::Common::Simulations::V2
