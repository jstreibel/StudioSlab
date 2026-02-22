#ifndef STUDIOSLAB_SIMULATION_SESSION_V2_H
#define STUDIOSLAB_SIMULATION_SESSION_V2_H

#include "Math/Numerics/V2/Events/SimulationEventV2.h"

namespace Slab::Math::Numerics::V2 {

    class FSimulationSessionV2 {
    public:
        virtual ~FSimulationSessionV2() = default;

        virtual auto InitializeForCurrentThread() -> void = 0;
        virtual auto Step(UIntBig nSteps) -> void = 0;

        [[nodiscard]] virtual auto GetCursor() const -> FSimulationCursorV2 = 0;
        [[nodiscard]] virtual auto GetCurrentState() const -> Base::EquationState_constptr = 0;
        [[nodiscard]] virtual auto SupportsSimulationTime() const -> bool = 0;
    };

    DefinePointers(FSimulationSessionV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_SIMULATION_SESSION_V2_H
