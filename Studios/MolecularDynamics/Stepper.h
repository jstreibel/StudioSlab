//
// Created by joao on 8/08/23.
//

#ifndef MOLDYN_VERLET_STEPPER_H
#define MOLDYN_VERLET_STEPPER_H

#include "Hamiltonians/SoftDisk/SoftDisk.h"
#include "Hamiltonians/Lennard-Jones/LennardJones.h"

#include "Particle.h"

#include "Phys/Numerics/Method/Stepper.h"
#include "Phys/Numerics/Program/NumericParams.h"


namespace MolecularDynamics {

    class VerletStepper : public Stepper {
        LennardJones lennardJones;
        SoftDisk softDisk;

        MoleculeContainer molecules;
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

#endif //MOLDYN_VERLET_STEPPER_H
