//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_RTORMODELALLOCATOR_H
#define FIELDS_RTORMODELALLOCATOR_H


#include "Phys/Numerics/Allocator.h"
#include "Mappings/RtoR/Model/RtoRFunction.h"

class RtoRModelAllocator : public Numerics::Allocator {

protected:
    RtoRModelAllocator();

public:
    enum Potential {V, KleinGordon, free, null} potential = V;

    static auto Choose() -> RtoRModelAllocator*;

    static void SetPotential(Potential pot, std::vector<Real> params = {});

    auto newFunctionArbitrary() -> void * override;
    auto newFieldState() -> void * override;
    auto getSystemSolver() -> void * override;


};


#endif //FIELDS_RTORMODELALLOCATOR_H
