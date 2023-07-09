//
// Created by joao on 09/06/23.
//

#include "Allocator.h"
#include "DrivenEquation.h"
#include "Common/Log/Log.h"

namespace Here = R2toR::LeadingDelta;

R2toR::LeadingDelta::Allocator::Allocator()
    : R2toR::Core::BasicAllocator(Str("2ⁿᵈ order non-homogenous eq. ℝ² ↦ ℝ with "
                                         "(yet unspecified) driving force.")) { }

auto R2toR::LeadingDelta::Allocator::getSystemSolver() -> void * {
    return new DrivenEquation(drivingFunction);
}

auto R2toR::LeadingDelta::Allocator::setDrivingFunction(Function::Ptr drivingFunction) -> void {
    Log::Info() << "LeadingDelta::Allocator setting up driving function: "
                << Log::ForegroundGreen << drivingFunction->myName() << Log::Flush;

    this->drivingFunction = drivingFunction;
}


