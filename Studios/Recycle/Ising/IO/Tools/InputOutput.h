//
// Created by joao on 20/05/2021.
//

#ifndef ISING_INPUTOUTPUT_H
#define ISING_INPUTOUTPUT_H


#include "../../Utils/ThermoUtils.h"
#include "../../IsingMonteCarlo/IsingNetwork.h"


namespace ThermoOutput {

    struct OutputData {
        const int mcTotalSteps;
        const int mcStep;
        const int transientSize;

        const IsingNetwork &S;

        OutputData(int mcTotalSteps, int mcStep, int transientSize, IsingNetwork &S, Real h)
            : mcTotalSteps(mcTotalSteps), mcStep(mcStep), transientSize(transientSize), S(S)
        {
            Real E = S.E(h), M = S.M();

            N = (double)S.N;
            e=E/N;
            m= fabs(M/N);
            e2=E*E/N;
            m2=M*M/N;
        }

        OutputData(const IsingNetwork &S, Real h) : mcTotalSteps(-1), mcStep(-1), transientSize(-1), S(S){
            Real E = S.E(double(h)), M = S.M();

            N = (double)S.N;
            e=E/N;
            m= fabs(M)/N;
            e2=e*e;
            m2=m*m;
        }

        Real e, m, e2, m2, N;
    };


// Tambem conhecido como parametros termodinamicos
    struct SystemParams {
        SystemParams(Real &T, Real &h, bool &shouldRun)
                : T(T), h(h), shouldRun(shouldRun) {}

        Real &T, &h;
        bool &shouldRun;
    };
}

#endif //ISING_INPUTOUTPUT_H
