

#include "input-shockwave.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/Shockwave/AnalyticShockwave1D.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"



namespace Studios::PureSG {

    InputShockwave::InputShockwave() : Builder("1d shockwave", "Shockwave in 1-dim") {
        interface->addParameters({&a0, &E});
    }

    auto InputShockwave::getBoundary() -> Math::Base::BoundaryConditions_ptr {
        //deltaType = vm["delta"].as<unsigned int>();

        auto a = 2 * a0.getValue();

        const Real eps = a * a / (3 * E.getValue());

        Slab::Math::RtoR::AnalyticShockwave1D shockwave1D(*a0);
        auto proto = newFieldState();
        return New<BoundaryCondition>(proto, new RtoR::NullFunction,
                                           new RtoR::RegularDiracDelta(eps, a, RtoR::RegularDiracDelta::Regularization(
                                                   deltaType)));
    }


}