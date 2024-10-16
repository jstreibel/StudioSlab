//
// Created by joao on 8/08/23.
//

#ifndef MOLDYN_VERLET_STEPPER_H
#define MOLDYN_VERLET_STEPPER_H

#include "Particle.h"

#include "Math/Numerics/Method/Stepper.h"

#include "Models/MolecularDynamics/MolDynNumericConfig.h"

namespace MolecularDynamics {

    using namespace Slab;

    template<class NewtonMechanicsModel>
    class VerletStepper : public Math::Stepper {
        NewtonMechanicsModel mechanicsModel;

        Graphics::PointContainer q, p;
        Pointer<State> state;
        Count currStep=0;

    public:
        using Config = Pointer<Models::MolecularDynamics::MolDynNumericConfig>;

        explicit VerletStepper(Config config, NewtonMechanicsModel mechModel);

        void step(const Real &dt, Count n_steps) override;

        auto getCurrentState() const -> Math::Base::EquationState_constptr override;
    };

}

#endif //MOLDYN_VERLET_STEPPER_H
