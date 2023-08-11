//
// Created by joao on 8/08/23.
//

#ifndef MOLDYN_VERLET_STEPPER_H
#define MOLDYN_VERLET_STEPPER_H

#include "Particle.h"

#include "Phys/Numerics/Method/Stepper.h"
#include "Phys/Numerics/Program/NumericParams.h"

namespace MolecularDynamics {

    template<class NewtonMechanicsModel>
    class VerletStepper : public Stepper {
        NewtonMechanicsModel mechanicsModel;

        PointContainer q, p;
        State state;
        Count currStep=0;

    public:

        explicit VerletStepper(NumericParams &params, NewtonMechanicsModel mechModel);

        void step(const Real &dt, Count n_steps) override;

        auto getCurrentState() const -> const void * override;

        auto getSpaces() const -> DiscreteSpacePair override;
    };

}

#endif //MOLDYN_VERLET_STEPPER_H
