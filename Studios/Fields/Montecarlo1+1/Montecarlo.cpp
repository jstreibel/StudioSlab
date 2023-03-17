//
// Created by joao on 3/17/23.
//


#include "Montecarlo.h"


#include "Fields/Mappings/RtoR/Model/RtoRFunction.h"
#include "Fields/Mappings/RtoR/Model/RtoRBoundaryCondition.h"
#include "Fields/Mappings/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"
#include "Fields/Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"

#include "Phys/Thermal/Utils/RandUtils.h"
#include "Phys/Numerics/Allocator.h"

namespace Montecarlo {



    const void *Input::getBoundary() const {

        auto L = Allocator::getInstance().getNumericParams().getL(); // not good bc 'L' is not my parameter.
        auto xLeft = Allocator::getInstance().getNumericParams().getxLeft();
        //auto L = 20.;

        auto oscLength = L / *n;
        auto oscEnergy = *E / *n;

        auto osc_eps = oscLength/2;
        ////auto osc_eps = a*a / (3*oscEnergy);
        auto a = sqrt(3*osc_eps*oscEnergy);
        auto a0= .5*a;


        RtoR::FunctionSummable dPhidt0;

        const auto ONE_plus = 1.+1.e-10;
        for(int i=0; i<*n; i++){
            auto tx = xLeft + ONE_plus*double(i)*(oscLength) + osc_eps;

            auto s = RandUtils::RandInt()%2?1.:-1.;

            dPhidt0 += RtoR::RegularDiracDelta(osc_eps, s*a, RtoR::RegularDiracDelta::Triangle, tx);
        }

        return new RtoR::BoundaryCondition(RtoR::NullFunction().Clone(), dPhidt0.Clone());

    }

} // RtoR