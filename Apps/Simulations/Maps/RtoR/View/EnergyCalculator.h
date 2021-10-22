//
// Created by joao on 17/04/2021.
//

#ifndef V_SHAPE_ENERGYCALCULATOR_H
#define V_SHAPE_ENERGYCALCULATOR_H

#include "Core/Allocator.h"
#include "Apps/Simulations/Maps/RtoR/Model/RtoRFunctionArbitrary.h"
#include "Apps/Simulations/Maps/RtoR/Model/RtoRFieldState.h"


namespace RtoR {
    class EnergyCalculator {
        // TODO colocar essa calculadora em .h e .cpp proprios
        RtoR::FunctionArbitrary *_oEnergyDensityFunc = static_cast<RtoR::FunctionArbitrary *>(Allocator::getInstance().newFunctionArbitrary());
    public:
        const RtoR::FunctionArbitrary &computeEnergyDensity(const RtoR::FieldState &field);

        const RtoR::FunctionArbitrary &getFunc() { return *_oEnergyDensityFunc; };

        Real integrate();

        Real integrate(Real xmin, Real xmax);
    };
}

#endif //V_SHAPE_ENERGYCALCULATOR_H
