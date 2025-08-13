//
// Created by joao on 16/05/2021.
//

#include "ThermoUtils.h"

#include <cmath>

namespace ThermoUtils {
                                                // T                e_T
    std::map<double, double> eT_Onsager_by_T = {{1.0, -1.997160204112251   },
                                                {1.5, -1.951116573078377   },
                                                {1.8, -1.859303805725896   },
                                                {2.0, -1.745564575312553   }};

    DevFloat f_Gibbs(DevFloat T, DevFloat Z, DevFloat N) { return -T * log(Z) / N; }

    DevFloat c_v(DevFloat T, DevFloat e_av, DevFloat e2_av, DevFloat N) {
        return N * (e2_av - e_av * e_av) / (T * T);
    }

    DevFloat chi(DevFloat T, DevFloat m_av, DevFloat m2_av, DevFloat N) { return N * (m2_av - m_av * m_av) / T; }

    DevFloat BoltzmannWeight_betaE(DevFloat betaE) { return exp(-betaE); }

    DevFloat BoltzmannWeight(DevFloat T, DevFloat E) { return exp(-E / T); }


    DevFloat current_T=-1.;
    DevFloat lookupTable[5];
    void GenerateBoltzmannWeightLookUpTable(DevFloat T){
        if(T==current_T) return;

        current_T = T;
        for(auto deltaE : {-8,-4,0,4,8})
            lookupTable[(deltaE+8)/4] = BoltzmannWeight(T, DevFloat(deltaE));
    }

    /**
     * Fast compute of deltaE using lookup table. IMPORTANT: external field h must be ZERO! Function WILL NOT assert
     * this. DeltaE should be one of -8, -4, 0, 4, 8.
     * @param deltaE the energy difference.
     * @return
     */
    DevFloat BoltzmannWeightDeltaE_h0(int deltaE) {
        return lookupTable[(deltaE+8)/4];
    }


}
