//
// Created by joao on 09/06/23.
//

#ifndef STUDIOSLAB_ALLOCATOR_H
#define STUDIOSLAB_ALLOCATOR_H

#include "Mappings/R2toR/Core/R2toR_Allocator.h"
#include "Mappings/R2toR/Model/R2toRFunction.h"

namespace R2toR {
    namespace LeadingDelta {
        class Allocator : public R2toR::Core::BasicAllocator {
            Function::Ptr drivingFunction;
            Allocator();

        public:
            static auto Choose()   -> Allocator*;

            auto getSystemSolver() -> void * override;
            auto setDrivingFunction(Function::Ptr drivingFunction) -> void;
        };

    }
}
#endif //STUDIOSLAB_ALLOCATOR_H
