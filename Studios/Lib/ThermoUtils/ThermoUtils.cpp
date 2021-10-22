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

    Real f_Gibbs(Real T, Real Z, Real N) { return -T * log(Z) / N; }

    Real c_v(Real T, Real e_av, Real e2_av, Real N) {
        return N * (e2_av - e_av * e_av) / (T * T);
    }

    Real chi(Real T, Real m_av, Real m2_av, Real N) { return N * (m2_av - m_av * m_av) / T; }

    Real BoltzmannWeight_betaE(Real betaE) { return exp(-betaE); }

    Real BoltzmannWeight(Real T, Real E) { return exp(-E / T); }


    Real current_T=-1.;
    Real lookupTable[5];
    void GenerateBoltzmannWeightIsingLookUpTable(Real T){
        if(T==current_T) return;

        current_T = T;
        for(auto deltaE : {-8,-4,0,4,8})
            lookupTable[(deltaE+8)/4] = BoltzmannWeight(T, Real(deltaE));
    }

    /**
     * Fast compute of deltaE using lookup table. IMPORTANT: external field h must be ZERO! Function WILL NOT assert
     * this. DeltaE should be one of -8, -4, 0, 4, 8.
     * @param deltaE the energy difference.
     * @return
     */
    Real BoltzmannWeightIsingDeltaE_h0(int deltaE) {
        return lookupTable[(deltaE+8)/4];
    }


}
