//
// Created by joao on 7/19/19.
//

#include "input-general-oscillons.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace Studios::PureSG {


    InputGeneralOscillons::InputGeneralOscillons()
            : Builder("Scattering", "General scattering of any two oscillons.") {
        interface->addParameters({&v1, &v2, &alpha1, &alpha2, &l, &V, &xTouchLocation, &mirror});
    }

    auto InputGeneralOscillons::getBoundary() -> Math::Base::BoundaryConditions_ptr {
        using namespace Slab::Math::RtoR;

        AnalyticOscillon oscRight = AnalyticOscillon(0.0, v1.getValue(), V.getValue(), alpha1.getValue(),
                                                     false, false);
        AnalyticOscillon oscLeft = AnalyticOscillon(0.0, v2.getValue(), V.getValue(), alpha2.getValue(),
                                                    true, mirror.getValue());

        auto initCondPhi = oscRight + oscLeft;
        auto initCondDPhiDt = oscRight.swap() + oscLeft.swap();

        auto proto = newFieldState();

        return New<BoundaryCondition>(proto, initCondPhi.Clone(), initCondDPhiDt.Clone());
    }


}