

#include "input-shockwave.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/Shockwave/AnalyticShockwave1D.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"



namespace Studios::PureSG {

    InputShockwave::InputShockwave() : Builder("1d shockwave", "Shockwave in 1-dim") {
        Interface->AddParameters({Naked(a0), Naked(E)});
    }

    auto InputShockwave::GetBoundary() -> Math::Base::BoundaryConditions_ptr {
        //deltaType = vm["delta"].as<unsigned int>();

        auto a = 2 * a0.GetValue();

        const DevFloat eps = a * a / (3 * E.GetValue());

        Slab::Math::RtoR::AnalyticShockwave1D shockwave1D(*a0);
        auto proto = NewFieldState();
        auto reggy = RtoR::RegularDiracDelta::Regularization(deltaType);
        auto DiracMateyMate = New <RtoR::RegularDiracDelta> (eps, a, reggy);

        return New<BoundaryCondition>(proto, New <RtoR::NullFunction> (), DiracMateyMate);
    }


}