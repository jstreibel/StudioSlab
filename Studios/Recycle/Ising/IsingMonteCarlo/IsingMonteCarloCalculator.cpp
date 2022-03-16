//
// Created by joao on 15/05/2021.
//

#include "IsingMonteCarloCalculator.h"

#include "../Utils/RandUtils.h"
#include "../Utils/ThermoUtils.h"

#include <cmath>
#include <iostream>
#include <numeric>
#include <boost/range/combine.hpp>

#define IS ==

#include "../COMPILE_CONFIG.h"


ThermoUtils::Real avg(const RealVector &v) {
    const ThermoUtils::Real zero = 0.0;
    const ThermoUtils::Real N = v.size();
    return std::accumulate(v.begin(),  v.end(),  zero)/N;
}


IsingMonteCarloCalculator::IsingMonteCarloCalculator(int L, ThermoUtils::Real T, ThermoUtils::Real h,
                                                     ThermoOutput::ViewControlBase *viewer, InitialConditions ic,
                                                     Dynamic dynamic, Sweeping sweeping)
: S(L), T(T), h(h), ic(ic), aDynamic(dynamic), vcOutput(viewer), sweeping(sweeping) {

#if OUTPUT_MODE IS CONSOLE_OUTPUT
    viewer.hide();
#endif

    _shake(0);
}


void IsingMonteCarloCalculator::_shake(double h) {
    if(ic == Ferromagnetic){
        auto val = RandUtils::RandInt()%2;
        for (int k = 0; k < S.N; k++)
            S.set(k, val);

    }
    else if(ic == Paramagnetic) {
        for (int k = 0; k < S.N; k++) {
            auto s = RandUtils::random01() <= 0.5 * (1 + h);
            S.set(k, s);
        }
    }
}


inline bool IsingMonteCarloCalculator::__shouldAccept(const ThermoUtils::Real deltaE) {
    if(deltaE<0) return true;

    const double r = RandUtils::random01();

    #if USE_LOOKUP_TABLE_FOR_DELTA_E == true
    const double z = ThermoUtils::BoltzmannWeightDeltaE_h0(deltaE);
    #else
    const double z = ThermoUtils::BoltzmannWeight((ThermoUtils::Real)T, (ThermoUtils::Real)deltaE);
    #endif

    return (r<z);
}


void IsingMonteCarloCalculator::MCStep() {
    switch(aDynamic)
    {
        case Metropolis:
            __MCStepMetropolis();
            break;
        case Kawasaki:
            __MCStepKawasaki();
            break;
    }
}

void IsingMonteCarloCalculator::__MCStepMetropolis() {
    if(sweeping == Random) {
        for (int ssf = 0; ssf < S.N; ++ssf) {
            // sorteio usando prob. (uniforme) do sitio estar na linha i:  P_i=1/L
            const int i = RandUtils::RandInt() % S.L;
            const int j = RandUtils::RandInt() % S.L;

            const double deltaE = S.ssfDeltaE(i, j, h);
            if (__shouldAccept(deltaE)) S.flip(i, j);
        }
    } else if(sweeping == Sequential) {
        for (int i = 0; i < S.L; ++i) {
            for (int j = 0; j < S.L; ++j) {
                const double deltaE = S.ssfDeltaE(i, j, h);
                if (__shouldAccept(deltaE)) S.flip(i, j);
            }
        }
    }
}
void IsingMonteCarloCalculator::__MCStepKawasaki() {
    auto N = S.N;

    for (int ssf = 0; ssf < N; ++ssf) {
        const auto s1 = RandUtils::RandInt() % N; // sorteio usando prob. (uniforme) do sitio estar na linha i:  P_i=1/L
        const auto s2 = RandUtils::RandInt() % N;

        if(S.s(s1)==S.s(s2)) continue;

        const double deltaE = S.tseDeltaE(s1, s2, h);

        if (__shouldAccept(deltaE)) {
            S.flip(s1);
            S.flip(s2);
        }
    }
}


void IsingMonteCarloCalculator::Simulate(int MCSteps, int transientSize) {
    _shake(h);

    std::cout.precision(8);
    const auto &N = S.N;
    const auto Nd = double(N);

#if OUTPUT_MODE IS CONSOLE_OUTPUT
    std::vector<ThermoUtils::Real> e, m, e2, m2, m4, Cv, Xi;

    for (int mcStep = 0; mcStep<MCSteps; ++mcStep) {
        if(mcStep < transientSize) continue; // descarta o transiente

        MCStep(T, h, N);

        ThermoUtils::Real E = S.E((double)h),
                          M = S.M();

        const auto _e = E/Nd, _m=M/Nd;
        e.push_back(_e);
        e2.push_back(_e*_e);
        m.push_back(abs(_m));
        m2.push_back(_m*_m);
        m4.push_back(_m*_m*_m*_m);
    } // end sim for

    std::cout << T << " ";
    _outputDataToConsole(e, e2, m, m2, m4, T, Nd);

#else
    bool shouldRun = false;
    bool firstStop = true;

    for (int mcStep = 0; true; ++mcStep) {

        if(mcStep==MCSteps && firstStop){ shouldRun = false; firstStop = false;}
        if(shouldRun) MCStep(); else --mcStep; // end if shouldRun

        ThermoOutput::SystemParams p = {T, h, shouldRun};
        ThermoOutput::OutputData data = {MCSteps, mcStep, transientSize, S, h};

        if (!vcOutput->doOperate(p, data)) exit(0);
    } // end sim for
#endif

}

void
IsingMonteCarloCalculator::_outputDataToConsole(const RealVector &e, const RealVector &e2, const RealVector &m,
                                                const RealVector &m2, const RealVector &m4, long double T, double N) {
    auto e_av = avg(e),
         e2_av= avg(e2),
         m_av = avg(m),
         m2_av= avg(m2),
         m4_av= avg(m4);

    auto Cv = (e2_av-e_av*e_av)*N/(T*T),
         Xi = (m2_av-m_av*m_av)*N/T,
         B = .5*(3-m4_av/(m2_av*m2_av));

    std::cout << e_av << " " << e2_av << " " << m_av << " " << m2_av << " " << m4_av << " " << Cv << " " << Xi << " " << B << "\n";
}

void IsingMonteCarloCalculator::set_T(ThermoUtils::Real T) {
    this->T = T;
    ThermoUtils::GenerateBoltzmannWeightLookUpTable(T);
}

void IsingMonteCarloCalculator::set_h(ThermoUtils::Real h) {
    this->h = h;
}



