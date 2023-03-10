//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_RTORMODELALLOCATOR_H
#define FIELDS_RTORMODELALLOCATOR_H


#include <Phys/Numerics/Allocator.h>

class RtoRModelAllocator : public Allocator {

protected:
    RtoRModelAllocator();

public:
    static auto Choose() -> RtoRModelAllocator*;

    auto newFunctionArbitrary() -> void * override;
    auto newFieldState() -> void * override;
    auto getSystemSolver() -> void * override;

};


#endif //FIELDS_RTORMODELALLOCATOR_H
