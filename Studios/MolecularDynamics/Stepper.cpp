//
// Created by joao on 8/08/23.
//

#include "Stepper.h"

MolecularDynamics::VerletStepper::VerletStepper(Count N)
: Stepper()
, q(N)
, p(N)
{

}

void MolecularDynamics::VerletStepper::step(const Real &dt, size_t n_steps) {

}

const void *MolecularDynamics::VerletStepper::getCurrentState() const {
    return nullptr;
}

DiscreteSpacePair MolecularDynamics::VerletStepper::getSpaces() const {
    return DiscreteSpacePair();
}

