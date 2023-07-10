//
// Created by joao on 3/13/23.
//

#ifndef STUDIOSLAB_RTORMODELALLOCATORMONTECARLO_H
#define STUDIOSLAB_RTORMODELALLOCATORMONTECARLO_H


#include "RtoRModelAllocator.h"

class RtoRModelAllocator_Montecarlo : public RtoRModelAllocator {
public:
    RtoRModelAllocator_Montecarlo();
    auto getSystemSolver() -> void * override;

};


#endif //STUDIOSLAB_RTORMODELALLOCATORMONTECARLO_H
