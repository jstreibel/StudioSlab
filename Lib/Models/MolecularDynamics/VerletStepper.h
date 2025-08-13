//
// Created by joao on 8/08/23.
//

#ifndef MOLDYN_VERLET_STEPPER_H
#define MOLDYN_VERLET_STEPPER_H

#include "Models/MolecularDynamics/Hamiltonians/MoleculesState.h"
#include "Models/MolecularDynamics/Hamiltonians/Particle.h"

#include "Math/Numerics/Stepper.h"

#include "Models/MolecularDynamics/MolDynNumericConfig.h"

namespace Slab::Models::MolecularDynamics {

    using namespace Slab;

    template<class NewtonMechanicsModel>
    class VerletStepper : public Math::Stepper {
        NewtonMechanicsModel mechanicsModel;

        Graphics::PointContainer q, p;
        TPointer<MoleculesState> state;
        CountType currStep=0;

        DevFloat dt;

    public:
        using Config = TPointer<Models::MolecularDynamics::MolDynNumericConfig>;

        explicit VerletStepper(Config config, NewtonMechanicsModel mechModel);

        void step(CountType n_steps) override;

        auto getCurrentState() const -> Math::Base::EquationState_constptr override;
    };

}

#endif //MOLDYN_VERLET_STEPPER_H
