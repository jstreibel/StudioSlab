//
// Created by joao on 17/04/2021.
//

#ifndef V_SHAPE_ENERGYCALCULATOR_H
#define V_SHAPE_ENERGYCALCULATOR_H

#include "Fields/Mappings/RtoR/Model/RtoRFunctionArbitrary.h"
#include "Fields/Mappings/RtoR/Model/RtoRFieldState.h"

#include "Phys/Numerics/Allocator.h"


namespace RtoR {
    class EnergyCalculator {
        RtoR::ArbitraryFunction *_oEnergyDensityFunc =
                static_cast<RtoR::ArbitraryFunction *>(Allocator::getInstance().newFunctionArbitrary());
        RtoR::ArbitraryFunction *_oKinetic =
                static_cast<RtoR::ArbitraryFunction *>(Allocator::getInstance().newFunctionArbitrary());
        RtoR::ArbitraryFunction *_oGradient =
                static_cast<RtoR::ArbitraryFunction *>(Allocator::getInstance().newFunctionArbitrary());
        RtoR::ArbitraryFunction *_oPotential =
                static_cast<RtoR::ArbitraryFunction *>(Allocator::getInstance().newFunctionArbitrary());
    public:
        const RtoR::ArbitraryFunction &computeDensities(const RtoR::FieldState &field);

        const RtoR::ArbitraryFunction &getEnergy() const { return *_oEnergyDensityFunc; };
        const RtoR::ArbitraryFunction &getKinetic() const { return *_oKinetic; };
        const RtoR::ArbitraryFunction &getGradient() const { return *_oGradient; };
        const RtoR::ArbitraryFunction &getPotential() const { return *_oPotential; };

        Real integrateEnergy();
        Real integrateKinetic();
        Real integrateGradient();
        Real integratePotential();

        Real integrateEnergy(Real xmin, Real xmax);
    };
}

#endif //V_SHAPE_ENERGYCALCULATOR_H
