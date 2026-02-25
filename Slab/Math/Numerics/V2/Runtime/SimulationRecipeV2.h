#ifndef STUDIOSLAB_SIMULATION_RECIPE_V2_H
#define STUDIOSLAB_SIMULATION_RECIPE_V2_H

#include "Math/Numerics/V2/Listeners/ListenerV2.h"
#include "Math/Numerics/V2/Runtime/SimulationSessionV2.h"

namespace Slab::Math::Numerics::V2 {

    class FSimulationRecipeV2 {
    public:
        virtual ~FSimulationRecipeV2() = default;

        virtual auto SetupForCurrentThread() -> void {}

        virtual auto BuildSession() -> TPointer<FSimulationSessionV2> = 0;
        virtual auto BuildDefaultSubscriptions() -> Vector<FSubscriptionV2> = 0;

        [[nodiscard]] virtual auto GetRunLimits() const -> FRunLimitsV2 = 0;
    };

    DefinePointers(FSimulationRecipeV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_SIMULATION_RECIPE_V2_H
