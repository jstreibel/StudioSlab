#include "input-perturbed.h"

#include "../../Model/RtoRBoundaryCondition.h"
#include "../../Model/FunctionsCollection/NullFunction.h"
#include "../../Model/FunctionsCollection/PerturbedOscillonTimeDerivative.h"


using namespace RtoR;

InputPerturbations::InputPerturbations()
: RtoRBCInterface("Perturbed oscillon in its reference frame")
{
    addParameters({&l, &eps});
};

auto InputPerturbations::getBoundary() const -> const void *
{
    return new RtoR::BoundaryCondition(new RtoR::NullFunction,
                                       new RtoR::PerturbedOscillonTimeDerivative(l.value(),
                                                                                 eps.value()));
}