//
// Created by joao on 8/08/23.
//

#ifndef STUDIOSLAB_STEPPER_H
#define STUDIOSLAB_STEPPER_H

#include "Phys/Numerics/Method/Stepper.h"
#include "Phys/Numerics/Program/NumericParams.h"

namespace MolecularDynamics {

    class VerletStepper : public Stepper {
        LennardJones physModelMolDynamic;
        PointContainer q, p;

    public:
        VerletStepper(NumericParams &params);

        void step(const Real &dt, size_t n_steps) override;

        auto getCurrentState() const -> const void * override;

        auto getSpaces() const -> DiscreteSpacePair override;
    };

}

#endif //STUDIOSLAB_STEPPER_H
