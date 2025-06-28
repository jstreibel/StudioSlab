//
// Created by joao on 20/05/2021.
//

#ifndef ISING_INPUTOUTPUT_H
#define ISING_INPUTOUTPUT_H


#include "Math/Thermal/ThermoUtils.h"
#include "Math/Thermal/XYNetwork.h"


namespace Slab::Lost::ThermoOutput {

    using namespace Math;

    struct OutputData {
        const XYNetwork &S;

        const int accepted=0, rejected=0;

        OutputData(XYNetwork &S, DevFloat h, int acceptedCount=0, int rejectedCount=0)
        : S(S)
        , accepted(acceptedCount)
        , rejected(rejectedCount)
        {
            DevFloat E = S.E(h), M = S.M();

            N = (double)S.N;
            e=E/N;
            m= fabs(M/N);
            e2=E*E/N;
            m2=M*M/N;
        }

        OutputData(const XYNetwork &S, DevFloat h) : S(S) {
            DevFloat E = S.E(double(h)), M = S.M();

            N = (double)S.N;
            e=E/N;
            m= fabs(M)/N;
            e2=e*e;
            m2=m*m;
        }

        DevFloat e, m, e2, m2, N;
    };


    // Tambem conhecido como parametros termodinamicos
    struct SystemParams {
        SystemParams(DevFloat &T, DevFloat &h, DevFloat &δ, bool &shouldRun, bool &shouldOverrelax)
                : T(T), h(h), δ(δ), shouldRun(shouldRun), shouldOverrelax(shouldOverrelax) {}

        DevFloat &T, &h, &δ;
        bool &shouldRun;
        bool &shouldOverrelax;
    };
}

#endif //ISING_INPUTOUTPUT_H
