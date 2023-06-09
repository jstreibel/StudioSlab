//
// Created by joao on 09/06/23.
//

#ifndef STUDIOSLAB_ALLOCATOR_H
#define STUDIOSLAB_ALLOCATOR_H

#include "Mappings/R2toR/Core/R2toR_Allocator.h"

namespace R2toR {
    namespace LeadingDelta {
        class Allocator : public R2toR::Core::BasicAllocator {
            Real ϵ, W₀, tₘₐₓ;
        public:
            Allocator(Real ϵ, Real W₀, Real tₘₐₓ);

            auto getSystemSolver() -> void * override;

            static auto Choose(Real ϵ, Real W₀, Real tₘₐₓ) -> Core::BasicAllocator *;
        };

    }
}
#endif //STUDIOSLAB_ALLOCATOR_H
