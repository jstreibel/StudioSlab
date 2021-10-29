//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_RTORMODELALLOCATOR_H
#define FIELDS_RTORMODELALLOCATOR_H


#include <Studios/CoreMath/Allocator.h>

class RtoRModelAllocator : public Allocator {
    RtoRModelAllocator();

public:
    static auto Choose() -> RtoRModelAllocator*;
    static void Init();

    auto newFunctionArbitrary() -> void * override;
    auto newFieldState() -> void * override;
    auto getSystemSolver() -> void * override;

};


#endif //FIELDS_RTORMODELALLOCATOR_H
