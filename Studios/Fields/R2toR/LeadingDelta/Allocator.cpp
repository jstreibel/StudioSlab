//
// Created by joao on 09/06/23.
//

#include "Allocator.h"
#include "DrivenEquation.h"

R2toR::LeadingDelta::Allocator::Allocator(R2toR::Function::Ptr drivingFunction)
    : Core::BasicAllocator(String("Non-homogenous eq. allocator ℝ² ↦ ℝ with '") + drivingFunction->myName()
                            + "' driving force"),
      drivingFunction(drivingFunction) {

}

auto R2toR::LeadingDelta::Allocator::getSystemSolver() -> void * {
    return new DrivenEquation(drivingFunction);
}

auto R2toR::LeadingDelta::Allocator::Choose(R2toR::Function::Ptr drivingFunction) -> void {
    auto *me = new LeadingDelta::Allocator(drivingFunction);

    Numerics::Allocator::Instantiate(me);
}


