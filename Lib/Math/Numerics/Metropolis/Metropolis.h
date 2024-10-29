//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_METROPOLIS_H
#define STUDIOSLAB_METROPOLIS_H

#include "Utils/Numbers.h"

namespace Slab::Math {

    using Temperature = Real;

    class Metropolis {
        Temperature T=0;

    public:
        explicit Metropolis(Temperature);

        bool should_accept(Real ΔE) const;
        virtual void step() = 0;
    };



} // Slab::Math

#endif //STUDIOSLAB_METROPOLIS_H
