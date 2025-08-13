//
// Created by joao on 7/19/19.
//

#include "input-general-oscillons.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoR-Recipe.h"

namespace Studios::PureSG {


    InputGeneralOscillons::InputGeneralOscillons()
            : Builder("Scattering", "General scattering of any two oscillons.") {
        Interface->AddParameters({&v1, &v2, &alpha1, &alpha2, &l, &V, &xTouchLocation, &mirror});
    }

    auto InputGeneralOscillons::GetBoundary() -> Math::Base::BoundaryConditions_ptr {
        using namespace Slab::Math::RtoR;

        AnalyticOscillon oscRight = AnalyticOscillon(0.0, v1.GetValue(), V.GetValue(), alpha1.GetValue(),
                                                     false, false);
        AnalyticOscillon oscLeft = AnalyticOscillon(0.0, v2.GetValue(), V.GetValue(), alpha2.GetValue(),
                                                    true, mirror.GetValue());

        auto initCondPhi = oscRight + oscLeft;
        auto initCondDPhiDt = oscRight.swap() + oscLeft.swap();

        auto proto = NewFieldState();

        return New<BoundaryCondition>(proto, initCondPhi.Clone(), initCondDPhiDt.Clone());
    }


}