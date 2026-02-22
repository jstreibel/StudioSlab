#ifndef STUDIOSLAB_STEPPER_SESSION_V2_H
#define STUDIOSLAB_STEPPER_SESSION_V2_H

#include "SimulationSessionV2.h"

#include "Math/Numerics/Stepper.h"

namespace Slab::Math::Numerics::V2 {

    class FStepperSessionV2 final : public FSimulationSessionV2 {
        FStepper_ptr Stepper;
        FSimulationCursorV2 Cursor;
        std::optional<DevFloat> Dt;

    public:
        explicit FStepperSessionV2(FStepper_ptr stepper,
                                   std::optional<DevFloat> dt = std::nullopt,
                                   UIntBig initialStep = 0,
                                   std::optional<DevFloat> initialSimulationTime = std::nullopt);

        auto InitializeForCurrentThread() -> void override;
        auto Step(UIntBig nSteps) -> void override;

        [[nodiscard]] auto GetCursor() const -> FSimulationCursorV2 override;
        [[nodiscard]] auto GetCurrentState() const -> Base::EquationState_constptr override;
        [[nodiscard]] auto SupportsSimulationTime() const -> bool override;
    };

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_STEPPER_SESSION_V2_H
