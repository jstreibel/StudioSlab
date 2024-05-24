//
// Created by joao on 17/04/2021.
//

#ifndef V_SHAPE_ENERGYCALCULATOR_H
#define V_SHAPE_ENERGYCALCULATOR_H

#include "Math/Numerics/VoidBuilder.h"
#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Math/Function/RtoR/Model/RtoRDiscreteFunction.h"
#include "KG-RtoREquationState.h"

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class KGEnergy {
        VoidBuilder &builder;

        RtoR::DiscreteFunction_ptr _oEnergyDensity;
        RtoR::DiscreteFunction_ptr _oKineticDensity;
        RtoR::DiscreteFunction_ptr _oGradientDensity;
        RtoR::DiscreteFunction_ptr _oPotentialDensity;

        Real U, K, W, V;

        RtoR::Function_ptr V_ptr;

    public:
        KGEnergy(VoidBuilder &builder, RtoR::Function_ptr potentialFunc);

        const RtoR::DiscreteFunction &computeEnergies(const EquationState &field);

        RtoR::DiscreteFunction_ptr getEnergyDensity() const { return _oEnergyDensity; };
        RtoR::DiscreteFunction_ptr getKineticDensity() const { return _oKineticDensity; };
        RtoR::DiscreteFunction_ptr getGradientDensity() const { return _oGradientDensity; };
        RtoR::DiscreteFunction_ptr getPotentialDensity() const { return _oPotentialDensity; };
        RtoR::Function_constptr getThePotential() const { return V_ptr; }

        Real getTotalEnergy() const;
        Real getTotalKineticEnergy() const;
        Real getTotalGradientEnergy() const;
        Real getTotalPotentialEnergy() const;

        Real integrateEnergy(Real xmin, Real xmax);
    };
}

#endif //V_SHAPE_ENERGYCALCULATOR_H
