//
// Created by joao on 04/04/2020.
//

#include "Mappings/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "Mappings/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"

#include <math.h>
#include "StatisticalBuilder.h"
#include "Common/RandUtils.h"

RtoR::StatisticalBuilder::StatisticalBuilder()
: RtoR::Thermal::Builder("Statistical", "Energy and density of oscillons statistical input")
{
    interface->addParameters({&E, &n});
}

auto RtoR::StatisticalBuilder::getDetailedDescription() -> Str {
    return Str("Fractality in the signum-Gordon field theory."
                  ""
                  "Oscillons are a very degenerate solution to the 1+1 dimensional signum-Gordon equation."
                  "The degeneracy could be an indication of how stable an oscillon could be: if it is very degenerate, "
                  "then a whole lot of different configurations could remain analitically stable, and a whole lot more "
                  "quasi-exact solutions could remain quasi-stable for even longer times. It could (see Arodz, Klimas "
                  "et al 2018 (or 2019 not sure) be the case that perturbed oscillons are very stable, so a big oscillon"
                  "on a bath of small oscillons (some sort of thermal equilibrium) could find subsidy to remain stable.");
}

auto RtoR::StatisticalBuilder::getBoundary() -> void * {
    auto L = simulationConfig.numericConfig.getL(); // not good bc 'L' is not my parameter.
    auto xLeft = simulationConfig.numericConfig.getxMin();

    auto oscLength = L / *n;
    auto oscEnergy = *E / *n;

    auto osc_eps = oscLength/2;
    ////auto osc_eps = a*a / (3*oscEnergy);
    auto a = sqrt(3*osc_eps*oscEnergy);
    auto a0= .5*a;


    RtoR::FunctionSummable dPhidt0;

    const auto ONE_plus = 1.+1.e-10;
    for(int i=0; i<*n; i++){
        auto tx = xLeft + ONE_plus*Real(i)*(oscLength) + osc_eps;

        auto s = RandUtils::RandInt()%2?1.:-1.;

        dPhidt0 += RtoR::RegularDiracDelta(osc_eps, s*a, RegularDiracDelta::Triangle, tx);
    }

    auto proto = (RtoR::EquationState*)newFieldState();
    return new BoundaryCondition(*proto, NullFunction().Clone(), dPhidt0.Clone());
}
