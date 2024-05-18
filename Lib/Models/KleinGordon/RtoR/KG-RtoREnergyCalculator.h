//
// Created by joao on 17/04/2021.
//

#ifndef V_SHAPE_ENERGYCALCULATOR_H
#define V_SHAPE_ENERGYCALCULATOR_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Core::Simulation{
    class VoidBuilder;
}

namespace RtoR {
    class EquationState;
    class DiscreteFunction;

    class KGEnergy {
        Core::Simulation::VoidBuilder &builder;

        RtoR::DiscreteFunction *_oEnergyDensity;
        RtoR::DiscreteFunction *_oKineticDensity;
        RtoR::DiscreteFunction *_oGradientDensity;
        RtoR::DiscreteFunction *_oPotentialDensity;

        Real U, K, W, V;

        RtoR::Function::Ptr V_ptr;

    public:
        KGEnergy(Core::Simulation::VoidBuilder &builder, RtoR::Function::Ptr potentialFunc);

        const RtoR::DiscreteFunction &computeEnergies(const RtoR::EquationState &field);

        const RtoR::DiscreteFunction &getEnergyDensity() const { return *_oEnergyDensity; };
        const RtoR::DiscreteFunction &getKineticDensity() const { return *_oKineticDensity; };
        const RtoR::DiscreteFunction &getGradientDensity() const { return *_oGradientDensity; };
        const RtoR::DiscreteFunction &getPotentialDensity() const { return *_oPotentialDensity; };
        const RtoR::Function::Ptr &getThePotential() const { return V_ptr; }

        Real getTotalEnergy() const;
        Real getTotalKineticEnergy() const;
        Real getTotalGradientEnergy() const;
        Real getTotalPotentialEnergy() const;

        Real integrateEnergy(Real xmin, Real xmax);
    };
}

#endif //V_SHAPE_ENERGYCALCULATOR_H
