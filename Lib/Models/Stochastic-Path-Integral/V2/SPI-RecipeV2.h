#ifndef STUDIOSLAB_SPI_RECIPE_V2_H
#define STUDIOSLAB_SPI_RECIPE_V2_H

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/Stepper.h"
#include "Math/Numerics/V2/Runtime/SimulationRecipeV2.h"

#include "../SPINumericConfig.h"

namespace Slab::Models::StochasticPathIntegrals::V2 {

    class FSPIRecipeV2 final : public Math::Numerics::V2::FSimulationRecipeV2 {
        TPointer<SPINumericConfig> NumericConfig;
        UIntBig ConsoleIntervalSteps = 1;
        UIntBig LiveViewIntervalSteps = 0; // 0 => follow ConsoleIntervalSteps
        TPointer<Math::LiveData::V2::FSessionLiveViewV2> LiveView = nullptr;

        auto BuildStepper() const -> TPointer<Math::FStepper>;

    public:
        explicit FSPIRecipeV2(const TPointer<SPINumericConfig> &numericConfig = New<SPINumericConfig>(),
                              UIntBig consoleIntervalSteps = 100,
                              const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView = nullptr);

        auto BuildSession() -> TPointer<Math::Numerics::V2::FSimulationSessionV2> override;
        auto BuildDefaultSubscriptions() -> Vector<Math::Numerics::V2::FSubscriptionV2> override;

        auto SetLiveViewIntervalSteps(UIntBig liveViewIntervalSteps) -> void;

        [[nodiscard]] auto GetRunLimits() const -> Math::Numerics::V2::FRunLimitsV2 override;
        [[nodiscard]] auto GetNumericConfig() const -> TPointer<SPINumericConfig>;
        [[nodiscard]] auto GetLiveView() const -> TPointer<Math::LiveData::V2::FSessionLiveViewV2>;
    };

    DefinePointers(FSPIRecipeV2)

} // namespace Slab::Models::StochasticPathIntegrals::V2

#endif // STUDIOSLAB_SPI_RECIPE_V2_H
