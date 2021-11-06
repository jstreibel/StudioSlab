//
// Created by joao on 17/04/2021.
//

#ifndef V_SHAPE_ENERGYCALCULATOR_H
#define V_SHAPE_ENERGYCALCULATOR_H

#include "../Model/RtoRFunctionArbitrary.h"
#include "../Model/RtoRFieldState.h"

#include <Phys/Numerics/Allocator.h>


namespace RtoR {
    class EnergyCalculator {
        RtoR::ArbitraryFunction *_oEnergyDensityFunc = static_cast<RtoR::ArbitraryFunction *>(Allocator::getInstance().newFunctionArbitrary());
    public:
        const RtoR::ArbitraryFunction &computeEnergyDensity(const RtoR::FieldState &field);

        const RtoR::ArbitraryFunction &getFunc() { return *_oEnergyDensityFunc; };

        Real integrate();

        Real integrate(Real xmin, Real xmax);
    };
}

#endif //V_SHAPE_ENERGYCALCULATOR_H
