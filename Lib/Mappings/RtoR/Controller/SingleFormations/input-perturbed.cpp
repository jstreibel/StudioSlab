#include "input-perturbed.h"

#include "Mappings/RtoR/Model/RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/PerturbedOscillonTimeDerivative.h"


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