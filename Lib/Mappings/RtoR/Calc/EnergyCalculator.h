//
// Created by joao on 17/04/2021.
//

#ifndef V_SHAPE_ENERGYCALCULATOR_H
#define V_SHAPE_ENERGYCALCULATOR_H

#include "Mappings/RtoR/Model/RtoRFunctionArbitrary.h"
#include "Mappings/RtoR/Model/RtoRFieldState.h"

#include "Phys/Numerics/Allocator.h"


namespace RtoR {
    class EnergyCalculator {
        RtoR::DiscreteFunction *_oEnergyDensityFunc =
                static_cast<RtoR::DiscreteFunction *>(Numerics::Allocator::GetInstance().newFunctionArbitrary());
        RtoR::DiscreteFunction *_oKinetic =
                static_cast<RtoR::DiscreteFunction *>(Numerics::Allocator::GetInstance().newFunctionArbitrary());
        RtoR::DiscreteFunction *_oGradient =
                static_cast<RtoR::DiscreteFunction *>(Numerics::Allocator::GetInstance().newFunctionArbitrary());
        RtoR::DiscreteFunction *_oPotential =
                static_cast<RtoR::DiscreteFunction *>(Numerics::Allocator::GetInstance().newFunctionArbitrary());
    public:
        const RtoR::DiscreteFunction &computeDensities(const RtoR::FieldState &field);

        const RtoR::DiscreteFunction &getEnergy() const { return *_oEnergyDensityFunc; };
        const RtoR::DiscreteFunction &getKinetic() const { return *_oKinetic; };
        const RtoR::DiscreteFunction &getGradient() const { return *_oGradient; };
        const RtoR::DiscreteFunction &getPotential() const { return *_oPotential; };

        Real integrateEnergy();
        Real integrateKinetic();
        Real integrateGradient();
        Real integratePotential();

        Real integrateEnergy(Real xmin, Real xmax);
    };
}

#endif //V_SHAPE_ENERGYCALCULATOR_H
