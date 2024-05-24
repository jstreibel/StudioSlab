#include "input-sym-oscillons.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"

namespace Studios::PureSG {
    
    using namespace Slab::Math;

    InputSymmetricOscillon::InputSymmetricOscillon()
            : PureSG::Builder("Symmetric scattering",
                              "Symmetric / antisymmetric oscillon scattering in (1+1) dimensions.") {
        interface->addParameters({&v, &V, &alpha, &isAntiSymmetric});
    }

    auto InputSymmetricOscillon::getBoundary() -> void * {
        RtoR::FunctionSummable &initCondPhi = *(new RtoR::FunctionSummable),
                &initCondDPhiDt = *(new RtoR::FunctionSummable);

        RtoR::AnalyticOscillon oscRight = RtoR::AnalyticOscillon(0.0, v.getValue(), V.getValue(), alpha.getValue(),
                                                                 false, false);
        RtoR::AnalyticOscillon oscLeft = RtoR::AnalyticOscillon(0.0, v.getValue(), V.getValue(), alpha.getValue(),
                                                                true, isAntiSymmetric.getValue());

        initCondPhi += oscRight + oscLeft;
        initCondDPhiDt += oscRight.swap() + oscLeft.swap();

        auto proto = (EquationState *) newFieldState();
        return new BoundaryCondition(*proto, &initCondPhi, &initCondDPhiDt);
    }

}