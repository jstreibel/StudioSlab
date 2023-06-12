//
// Created by joao on 09/06/23.
//

#include "Allocator.h"
#include "DrivenEquation.h"
#include "Common/Log/Log.h"

namespace Here = R2toR::LeadingDelta;

Here::Allocator* Here::Allocator::me = nullptr;

R2toR::LeadingDelta::Allocator::Allocator()
    : R2toR::Core::BasicAllocator(String("2ⁿᵈ order non-homogenous eq. ℝ² ↦ ℝ with "
                                         "(yet unspecified) driving force.")) { }

auto R2toR::LeadingDelta::Allocator::getSystemSolver() -> void * {
    return new DrivenEquation(drivingFunction);
}

auto R2toR::LeadingDelta::Allocator::Choose() -> Allocator* {
    if(me == nullptr) {
        me = new LeadingDelta::Allocator();
        Numerics::Allocator::Instantiate(me);
    }

    return me;

}

auto R2toR::LeadingDelta::Allocator::setDrivingFunction(Function::Ptr drivingFunction) -> void {
    Log::Info() << "LeadingDelta::Allocator setting up driving function: "
                << Log::ForegroundGreen << drivingFunction->myName() << Log::Flush;

    this->drivingFunction = drivingFunction;
}


