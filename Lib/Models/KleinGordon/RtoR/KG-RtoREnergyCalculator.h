//
// Created by joao on 17/04/2021.
//

#ifndef V_SHAPE_ENERGYCALCULATOR_H
#define V_SHAPE_ENERGYCALCULATOR_H

#include "Mappings/RtoR/Model/RtoRDiscreteFunction.h"
#include "KG-RtoREquationState.h"
#include "Phys/Numerics/VoidBuilder.h"

namespace RtoR {
    class KGEnergy {
        Base::Simulation::VoidBuilder &builder;

        RtoR::DiscreteFunction *_oEnergyDensityFunc;
        RtoR::DiscreteFunction *_oKinetic;
        RtoR::DiscreteFunction *_oGradient;
        RtoR::DiscreteFunction *_oPotential;

    public:
        KGEnergy(Base::Simulation::VoidBuilder &builder);

        const RtoR::DiscreteFunction &computeDensities(const RtoR::EquationState &field);

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
