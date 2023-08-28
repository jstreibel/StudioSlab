//
// Created by joao on 8/08/23.
//

#ifndef MOLDYN_VERLET_STEPPER_H
#define MOLDYN_VERLET_STEPPER_H

#include "Particle.h"

#include "Math/Numerics/Method/Stepper.h"
#include "Math/Numerics/SimConfig/NumericConfig.h"

namespace MolecularDynamics {

    template<class NewtonMechanicsModel>
    class VerletStepper : public Stepper {
        NewtonMechanicsModel mechanicsModel;

        PointContainer q, p;
        State state;
        Count currStep=0;

    public:

        explicit VerletStepper(NumericConfig &params, NewtonMechanicsModel mechModel);

        void step(const Real &dt, Count n_steps) override;

        auto getCurrentState() const -> const void * override;

        auto getSpaces() const -> DiscreteSpacePair override;
    };

}

#endif //MOLDYN_VERLET_STEPPER_H
