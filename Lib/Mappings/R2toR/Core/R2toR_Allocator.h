//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_R2TORMODELBUILDER_H
#define FIELDS_R2TORMODELBUILDER_H


#include "Phys/Numerics/Allocator.h"


namespace R2toR {

    namespace Core {
        class BasicAllocator : public Numerics::Allocator {
        public:
            BasicAllocator(String name = "Basic allocator ℝ² ↦ ℝ");

            static auto Choose()        -> BasicAllocator*;

            auto newFunctionArbitrary() -> void * override;
            auto newFieldState()        -> void * override;
            auto getSystemSolver()      -> void * override;
        };
    }
}


#endif //FIELDS_R2TORMODELBUILDER_H
