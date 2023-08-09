//
// Created by joao on 8/08/23.
//

#ifndef STUDIOSLAB_STEPPER_H
#define STUDIOSLAB_STEPPER_H

#include "Phys/Numerics/Method/Stepper.h"
#include "Phys/Numerics/Program/NumericParams.h"

#include "Hamiltonians/Lennard-Jones/LennardJones.h"

namespace MolecularDynamics {

    class VerletStepper : public Stepper {
    public:
        typedef std::pair<PointContainer &, PointContainer &> State;

    private:
        LennardJones physModelMolDynamic;
        PointContainer q, p;
        State state;
        Count currStep=0;

    public:

        VerletStepper(NumericParams &params);

        void step(const Real &dt, Count n_steps) override;

        auto getCurrentState() const -> const void * override;

        auto getSpaces() const -> DiscreteSpacePair override;
    };

}

#endif //STUDIOSLAB_STEPPER_H
