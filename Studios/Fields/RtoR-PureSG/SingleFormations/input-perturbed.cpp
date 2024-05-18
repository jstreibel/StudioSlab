#include "input-perturbed.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/PerturbedOscillonTimeDerivative.h"

using namespace RtoR;

InputPerturbations::InputPerturbations()
: KGBuilder("Perturbations", "Perturbed oscillon in its reference frame")
{
    interface->addParameters({&l, &eps});
};

auto InputPerturbations::getBoundary() -> void *
{
    auto proto = (RtoR::EquationState*)newFieldState();
    return new RtoR::BoundaryCondition(*proto, new RtoR::NullFunction,
                                       new RtoR::PerturbedOscillonTimeDerivative(l.getValue(),
                                                                                 eps.getValue()));
}