#include "input-perturbed.h"

#include "Model/math-util.h"
#include "Apps/Simulations/Maps/RtoR/Model/RtoRFieldState.h"
#include "Apps/Simulations/Maps/RtoR/Model/RtoRBoundaryCondition.h"
#include "Apps/Simulations/Maps/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Apps/Simulations/Maps/RtoR/Model/FunctionsCollection/PerturbedOscillonTimeDerivative.h"

#include "Core/Util/Workaround/StringStream.h"


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