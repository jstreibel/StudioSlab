#include "input-perturbed.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/PerturbedOscillonTimeDerivative.h"


namespace Studios::PureSG {

    using namespace Slab::Math;

    InputPerturbations::InputPerturbations()
            : Builder("Perturbations", "Perturbed oscillon in its reference frame") {
        interface->addParameters({&l, &eps});
    };

    auto InputPerturbations::getBoundary() -> Models::KGRtoR::BoundaryCondition_ptr {
        auto proto = newFieldState();
        return New<BoundaryCondition>(*proto, new RtoR::NullFunction,
                                              new RtoR::PerturbedOscillonTimeDerivative(l.getValue(),
                                                                                     eps.getValue()));
    }


}