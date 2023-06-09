//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_R2TORMODELBUILDER_H
#define FIELDS_R2TORMODELBUILDER_H


#include "Phys/Numerics/Allocator.h"


class Allocator_R2toR : public Numerics::Allocator {
public:
    Allocator_R2toR();

    static auto Choose()        -> Allocator_R2toR*;

    auto newFunctionArbitrary() -> void * override;
    auto newFieldState()        -> void * override;
    auto getSystemSolver()      -> void * override;
};


#endif //FIELDS_R2TORMODELBUILDER_H
