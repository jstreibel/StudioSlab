//
// Created by joao on 05/08/2021.
//

#include "FixedSizeMoleculeContainer.h"


void FixedSizeMoleculeContainer::reset() {
    occupation = 0;
}

void FixedSizeMoleculeContainer::add(VerletPointTriple triple) {
    if(occupation > HASH_CONTAINER_SIZE) {
        std::cout << "Max occupancy reached: " << occupation << "/" << HASH_CONTAINER_SIZE << std::endl;
        throw "FixedSizeMoleculeContainer max occupancy reached.";
    }

    triples[occupation] = triple;

    occupation++;
}

