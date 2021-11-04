//
// Created by joao on 20/05/2021.
//

#ifndef ISING_INPUTOUTPUT_H
#define ISING_INPUTOUTPUT_H


#include "Phys/Thermal/Utils/ThermoUtils.h"
#include "../../Model/XYNetwork.h"


namespace ThermoOutput {

    struct OutputData {
        const int mcTotalSteps;
        const int mcStep;
        const int transientSize;

        const XYNetwork &S;

        const int accepted=0, rejected=0;

        OutputData(int mcTotalSteps, int mcStep, int transientSize, XYNetwork &S, ThermoUtils::Real h,
                   int acceptedCount=0, int rejectedCount=0)
            : mcTotalSteps(mcTotalSteps), mcStep(mcStep), transientSize(transientSize), S(S),
              accepted(acceptedCount), rejected(rejectedCount)
        {
            ThermoUtils::Real E = S.E(h), M = S.M();

            N = (double)S.N;
            e=E/N;
            m= fabs(M/N);
            e2=E*E/N;
            m2=M*M/N;
        }

        OutputData(const XYNetwork &S, ThermoUtils::Real h) : mcTotalSteps(-1), mcStep(-1), transientSize(-1), S(S){
            ThermoUtils::Real E = S.E(double(h)), M = S.M();

            N = (double)S.N;
            e=E/N;
            m= fabs(M)/N;
            e2=e*e;
            m2=m*m;
        }

        ThermoUtils::Real e, m, e2, m2, N;
    };


// Tambem conhecido como parametros termodinamicos
    struct SystemParams {
        SystemParams(ThermoUtils::Real &T, ThermoUtils::Real &h, ThermoUtils::Real &δ, bool &shouldRun, bool &shouldOverrelax)
                : T(T), h(h), δ(δ), shouldRun(shouldRun), shouldOverrelax(shouldOverrelax) {}

        ThermoUtils::Real &T, &h, &δ;
        bool &shouldRun;
        bool &shouldOverrelax;
    };
}

#endif //ISING_INPUTOUTPUT_H
