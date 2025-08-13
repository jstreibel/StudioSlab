

#include "input-sym-oscillons.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"

namespace Studios::PureSG {
    
    using namespace Slab::Math;

    InputSymmetricOscillon::InputSymmetricOscillon()
            : PureSG::Builder("Symmetric scattering",
                              "Symmetric / antisymmetric oscillon scattering in (1+1) dimensions.") {
        Interface->AddParameters({&v, &V, &alpha, &isAntiSymmetric});
    }

    auto InputSymmetricOscillon::GetBoundary() -> Math::Base::BoundaryConditions_ptr {
        auto initCondPhi    = New <RtoR::FunctionSummable> ();
        auto initCondDPhiDt = New <RtoR::FunctionSummable> ();

        RtoR::AnalyticOscillon oscRight = RtoR::AnalyticOscillon(0.0, v.GetValue(), V.GetValue(), alpha.GetValue(),
                                                                 false, false);
        RtoR::AnalyticOscillon oscLeft = RtoR::AnalyticOscillon(0.0, v.GetValue(), V.GetValue(), alpha.GetValue(),
                                                                true, isAntiSymmetric.GetValue());

        *initCondPhi += oscRight + oscLeft;
        *initCondDPhiDt += oscRight.swap() + oscLeft.swap();

        auto proto = NewFieldState();
        return New<BoundaryCondition>(proto, initCondPhi, initCondDPhiDt);
    }

}