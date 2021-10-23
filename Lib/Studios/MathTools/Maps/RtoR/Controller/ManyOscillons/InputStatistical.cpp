//
// Created by joao on 04/04/2020.
//

#include <Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/AnalyticOscillon.h>
#include <Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/RegularDiracDelta.h>
#include <Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/NullFunction.h>

#include <Studios/MathTools/Maps/RtoR/Model/RtoRBoundaryCondition.h>

#include <math.h>
#include "RandUtils.h"
#include "InputStatistical.h"


RtoR::InputStatistical::InputStatistical()
: RtoRBCInterface("Energy and density of oscillons statistical input.")
{
    addParameters({&E, &n});
}

auto RtoR::InputStatistical::getDetailedDescription() -> String {
    return String("Fractality in the signum-Gordon field theory."
                  ""
                  "Oscillons are a very degenerate solution."
                  "The degeneracy could be an indication of how stable an oscillon could be: if it is very degenerate, "
                  "then a whole lot of different configurations could remain analitically stable, and a whole lot more "
                  "quasi-exact solutions could remain quasi-stable for even longer times. It could (see Arodz, Klimas "
                  "et al 2018 (or 2019 not sure) be the case that perturbed oscillons are very stable, so a big oscillon"
                  "on a bath of small oscillons (some sort of thermal equilibrium) could find subsidy to remain stable.");
}

auto RtoR::InputStatistical::getBoundary() const -> const void * {
    //auto L = vm["L"].as<double>(); // not good bc 'L' is not my parameter.
    auto L = 20.;
    std::cout << "Warning: statistical IC are using fixed artifical L=" << L << "value.";

    auto oscLength = L / *n;
    auto oscEnergy = *E / *n;

    auto osc_eps = oscLength/2;
    ////auto osc_eps = a*a / (3*oscEnergy);
    auto a = sqrt(3*osc_eps*oscEnergy);
    auto a0= .5*a;


    RtoR::FunctionSummable dPhidt0;

    const auto ONE_plus = 1.+1.e-10;
    for(int i=0; i<*n; i++){
        auto tx = -L/2 + ONE_plus*double(i)*(oscLength) + osc_eps;

        auto s = RandUtils::RandInt()%2?1.:-1.;

        dPhidt0 += RtoR::RegularDiracDelta(osc_eps, s*a, RegularDiracDelta::Triangle, tx);
    }

    return new BoundaryCondition(NullFunction().Clone(), dPhidt0.Clone());
}
