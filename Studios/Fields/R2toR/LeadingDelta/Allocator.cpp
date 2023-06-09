//
// Created by joao on 09/06/23.
//

#include "Allocator.h"
#include "DrivenEquation.h"

R2toR::LeadingDelta::Allocator::Allocator(Real ϵ, Real W₀, Real tₘₐₓ)
    : Core::BasicAllocator("Driven-δ allocator ℝ² ↦ ℝ"), ϵ(ϵ), W₀(W₀), tₘₐₓ(tₘₐₓ) {

}

auto R2toR::LeadingDelta::Allocator::getSystemSolver() -> void * {
    return new DrivenEquation(ϵ, W₀, tₘₐₓ);
}

auto R2toR::LeadingDelta::Allocator::Choose(Real ϵ, Real W₀, Real tₘₐₓ) -> Core::BasicAllocator * {
    auto *me = new LeadingDelta::Allocator(ϵ, W₀, tₘₐₓ);

    Numerics::Allocator::Instantiate(me);

    return me;
}


