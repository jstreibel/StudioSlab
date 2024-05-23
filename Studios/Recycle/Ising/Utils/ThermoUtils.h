//
// Created by joao on 16/05/2021.
//

#ifndef ISING_THERMOUTILS_H
#define ISING_THERMOUTILS_H

#include <boost/multiprecision/float128.hpp>
#include <boost/math/special_functions/expint.hpp>

namespace ThermoUtils {
    extern std::map<double, double> eT_Onsager_by_T;

    // typedef long double Real;

    Real BoltzmannWeight_betaE(Real betaE);
    Real BoltzmannWeight(Real T, Real E);

    void GenerateBoltzmannWeightLookUpTable(Real T);
    Real BoltzmannWeightDeltaE_h0(int deltaE);

    Real f_Gibbs  (Real T, Real Z, Real N);
    Real c_v(Real T, Real e_av, Real e2_av, Real N);
    Real chi(Real T, Real m_av, Real m2_av, Real N);


}

#endif //ISING_THERMOUTILS_H
