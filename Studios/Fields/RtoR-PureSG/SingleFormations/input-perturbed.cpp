#include "input-perturbed.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/PerturbedOscillonTimeDerivative.h"


namespace Studios::PureSG {

    using namespace Slab::Math;

    InputPerturbations::InputPerturbations()
            : Builder("Perturbations", "Perturbed oscillon in its reference frame") {
        interface->addParameters({Naked(l), Naked(eps)});
    };

    auto InputPerturbations::getBoundary() -> Math::Base::BoundaryConditions_ptr {
        auto proto = newFieldState();

        auto pertOscillon = New <RtoR::PerturbedOscillonTimeDerivative> (l.getValue(), eps.getValue());

        return New<BoundaryCondition>(proto,
                                      New <RtoR::NullFunction> (),
                                      pertOscillon);
    }


}