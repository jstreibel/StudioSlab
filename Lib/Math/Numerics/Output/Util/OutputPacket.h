#ifndef OUTPUTINFO_H
#define OUTPUTINFO_H

#include "FieldStateOutputInterface.h"

#include "Math/DifferentialEquations/EquationState.h"

#include <memory>
#include <utility>


namespace Slab::Math {

    class OutputPacket {
        Base::EquationState_constptr stateData = nullptr;
        size_t currentStep = 0;
        Real t = 0.0;

    public:
        OutputPacket() = default;
        OutputPacket(const OutputPacket &packet) : OutputPacket(packet.stateData, packet.currentStep, packet.t) {}
        OutputPacket(Base::EquationState_constptr stateData, size_t currentStep, floatt t)
        : stateData(std::move(stateData))
        , currentStep(currentStep)
        , t(t) {}

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


        inline auto getSteps()   const -> size_t { return currentStep; }

        inline auto getSimTime() const -> Real   { return t; }

    };


}

#endif // OUTPUTINFO_H
