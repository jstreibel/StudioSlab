//
// Created by joao on 16/05/2021.
//

#ifndef ISING_THERMOUTILS_H
#define ISING_THERMOUTILS_H

#include "Utils/Types.h"

//#include <boost/multiprecision/float128.hpp>
#include <boost/math/special_functions/expint.hpp>
#include <map>

namespace Slab::Math::ThermoUtils {
    extern std::map<DevFloat, DevFloat> eT_Onsager_by_T;

    DevFloat BoltzmannWeight_betaE(DevFloat betaE);
    DevFloat BoltzmannWeight(DevFloat T, DevFloat E);

    void GenerateBoltzmannWeightIsingLookUpTable(DevFloat T);
    DevFloat BoltzmannWeightIsingDeltaE_h0(int deltaE);

    DevFloat f_Gibbs  (DevFloat T, DevFloat Z, DevFloat N);
    DevFloat c_v(DevFloat T, DevFloat e_av, DevFloat e2_av, DevFloat N);
    DevFloat chi(DevFloat T, DevFloat m_av, DevFloat m2_av, DevFloat N);


}

#endif //ISING_THERMOUTILS_H
