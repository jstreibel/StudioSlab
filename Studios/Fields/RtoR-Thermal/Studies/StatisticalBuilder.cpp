//
// Created by joao on 04/04/2020.
//

#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"

#include <math.h>
#include "StatisticalBuilder.h"
#include "Utils/RandUtils.h"


namespace Studios::Fields::RtoRThermal {

    using namespace Slab;
    using namespace Slab::Math;

    StatisticalBuilder::StatisticalBuilder()
            : Builder("Statistical", "Energy and density of oscillons statistical input") {
        interface->addParameters({&E, &n});
    }

    auto StatisticalBuilder::getDetailedDescription() -> Str {
        return Str("Fractality in the signum-Gordon field theory."
                   ""
                   "Oscillons are a very degenerate solution to the 1+1 dimensional signum-Gordon equation."
                   "The degeneracy could be an indication of how stable an oscillon could be: if it is very degenerate, "
                   "then a whole lot of different configurations could remain analitically stable, and a whole lot more "
                   "quasi-exact solutions could remain quasi-stable for even longer times. It could (see Arodz, Klimas "
                   "et al 2018 (or 2019 not sure) be the case that perturbed oscillons are very stable, so a big oscillon"
                   "on a bath of small oscillons (some sort of thermal equilibrium) could find subsidy to remain stable.");
    }

    auto StatisticalBuilder::getBoundary() -> Base::BoundaryConditions_ptr {
        auto L = kg_numeric_config->getL(); // not good bc 'L' is not my parameter.
        auto xLeft = kg_numeric_config->getxMin();

        auto oscLength = L / *n;
        auto oscEnergy = *E / *n;

        auto osc_eps = oscLength / 2;
        ////auto osc_eps = a*a / (3*oscEnergy);
        auto a = sqrt(3 * osc_eps * oscEnergy);


        RtoR::FunctionSummable dPhidt0;

        const auto ONE_plus = 1. + 1.e-10;
        for (int i = 0; i < *n; i++) {
            auto tx = xLeft + ONE_plus * Real(i) * (oscLength) + osc_eps;

            auto s = RandUtils::RandomUniformUInt() % 2 ? 1. : -1.;

            dPhidt0 += RtoR::RegularDiracDelta(osc_eps, s * a, RtoR::RegularDiracDelta::Triangle, tx);
        }

        using namespace Slab::Models::KGRtoR;
        auto proto = newFieldState();
        return New<KGRtoR::BoundaryCondition>(proto, RtoR::NullFunction().Clone(), dPhidt0.Clone());
    }


}