//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_RTORMODELALLOCATORLANGEVIN_H
#define FIELDS_RTORMODELALLOCATORLANGEVIN_H


#include "RtoRModelAllocator.h"

class RtoRModelAllocator_Langevin : public RtoRModelAllocator {
    RtoRModelAllocator_Langevin() = default;

public:
    static auto Choose() -> RtoRModelAllocator_Langevin*;

    auto getSystemSolver() -> void * override;

};


#endif //FIELDS_RTORMODELALLOCATORLANGEVIN_H
