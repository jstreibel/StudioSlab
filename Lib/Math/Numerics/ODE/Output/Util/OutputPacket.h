#ifndef OUTPUTINFO_H
#define OUTPUTINFO_H

#include "FieldStateOutputInterface.h"

#include "Math/Numerics/ODE/Solver/EquationState.h"

#include <memory>
#include <utility>


namespace Slab::Math {

    class OutputPacket {
        Pointer<const Base::EquationState> stateData = nullptr;
        size_t currentStep = 0;

    public:
        OutputPacket() = default;
        OutputPacket(const OutputPacket &packet) : OutputPacket(packet.stateData, packet.currentStep) {}
        OutputPacket(Base::EquationState_constptr stateData, size_t currentStep)
        : stateData(std::move(stateData))
        , currentStep(currentStep)
        {}

        /*!
         * Make a copy of this class. Note that the EqStateOutputInterface is _not_ copied (it is not owned by this class
         * anyway).
         * @param oi The OutputInfo to copy.
         * @return LHS of equality.
         */
        OutputPacket &operator=(const OutputPacket &otherPacky) = default;

        bool hasValidData() {
            return stateData != nullptr;
        }

        template<typename EqStateType>
        [[nodiscard]] const EqStateType* GetNakedStateData() const {
            auto nakedState = stateData.get();

            return dynamic_cast<const EqStateType*>(nakedState);
        }

        [[nodiscard]] Str getStateCategory() const {
            if (stateData == nullptr) { return ""; }

            return stateData->category();
        };

        [[nodiscard]] inline auto getSteps()   const -> size_t { return currentStep; }

    };
}

#endif // OUTPUTINFO_H
