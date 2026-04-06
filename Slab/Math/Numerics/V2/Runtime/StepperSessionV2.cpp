#include "StepperSessionV2.h"

namespace Slab::Math::Numerics::V2 {

    FStepperSessionV2::FStepperSessionV2(FStepper_ptr stepper,
                                         const std::optional<DevFloat> dt,
                                         const UIntBig initialStep,
                                         const std::optional<DevFloat> initialSimulationTime)
    : Stepper(std::move(stepper))
    , Dt(dt) {
        Cursor.Step = initialStep;
        Cursor.SimulationTime = initialSimulationTime;
    }

    auto FStepperSessionV2::InitializeForCurrentThread() -> void {
        // Nothing to do for the generic stepper wrapper in Phase A.
    }

    auto FStepperSessionV2::StepUnsafe(const UIntBig nSteps) -> bool {
        if (Stepper == nullptr || nSteps == 0) return false;

        Stepper->Step(static_cast<size_t>(nSteps));
        Cursor.Step += nSteps;

        if (Dt.has_value()) {
            if (!Cursor.SimulationTime.has_value()) Cursor.SimulationTime = 0.0;
            *Cursor.SimulationTime += *Dt * static_cast<DevFloat>(nSteps);
        }

        return true;
    }

    auto FStepperSessionV2::GetCursorUnsafe() const -> FSimulationCursorV2 {
        return Cursor;
    }

    auto FStepperSessionV2::GetCurrentStateUnsafe() const -> Base::EquationState_constptr {
        return Stepper != nullptr ? Stepper->GetCurrentState() : nullptr;
    }

    auto FStepperSessionV2::SupportsSimulationTime() const -> bool {
        if (Dt.has_value()) return true;
        return GetCursor().SimulationTime.has_value();
    }

} // namespace Slab::Math::Numerics::V2
