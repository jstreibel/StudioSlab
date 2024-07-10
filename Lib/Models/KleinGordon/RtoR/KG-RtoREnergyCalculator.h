//
// Created by joao on 17/04/2021.
//

#ifndef V_SHAPE_ENERGYCALCULATOR_H
#define V_SHAPE_ENERGYCALCULATOR_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"
#include "KG-RtoREquationState.h"

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class KGEnergy {

        RtoR::NumericFunction_ptr _oEnergyDensity;
        RtoR::NumericFunction_ptr _oKineticDensity;
        RtoR::NumericFunction_ptr _oGradientDensity;
        RtoR::NumericFunction_ptr _oPotentialDensity;

        Real U, K, W, V;

        RtoR::Function_ptr V_ptr;

    public:
        KGEnergy(RtoR::Function_ptr potentialFunc);

        const RtoR::NumericFunction &computeEnergies(const RtoR::NumericFunction& phi, const RtoR::NumericFunction& ddtPhi);

        RtoR::NumericFunction_ptr getEnergyDensity() const { return _oEnergyDensity; };
        RtoR::NumericFunction_ptr getKineticDensity() const { return _oKineticDensity; };
        RtoR::NumericFunction_ptr getGradientDensity() const { return _oGradientDensity; };
        RtoR::NumericFunction_ptr getPotentialDensity() const { return _oPotentialDensity; };
        RtoR::Function_constptr getThePotential() const { return V_ptr; }

        Real getTotalEnergy() const;
        Real getTotalKineticEnergy() const;
        Real getTotalGradientEnergy() const;
        Real getTotalPotentialEnergy() const;

        Real integrateEnergy(Real xmin, Real xmax);
    };
}

#endif //V_SHAPE_ENERGYCALCULATOR_H
