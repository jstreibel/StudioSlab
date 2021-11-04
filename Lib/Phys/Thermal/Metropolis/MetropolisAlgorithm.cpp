//
// Created by joao on 15/05/2021.
//

#include "MetropolisAlgorithm.h"

#include "Phys/Thermal/Utils/RandUtils.h"
#include "Phys/Thermal/Utils/ThermoUtils.h"

#include <cmath>
#include <iostream>
#include <numeric>
#include <boost/range/combine.hpp>

#define IS ==

#include "Studios/Backend/SFML-Nuklear/COMPILE_CONFIG.h"


ThermoUtils::Real avg(const RealVector &v) {
    const ThermoUtils::Real zero = 0.0;
    const ThermoUtils::Real N = v.size();
    return std::accumulate(v.begin(),  v.end(),  zero)/N;
}


MetropolisAlgorithm::MetropolisAlgorithm(int L, ThermoUtils::Real T, ThermoUtils::Real h,
                                                     ThermoOutput::ViewControlBase *viewer, InitialConditions ic,
                                                     Dynamic dynamic, Sweeping sweeping)
: S(L), T(T), h(h), ic(ic), aDynamic(dynamic), vcOutput(viewer), sweeping(sweeping) {

#if OUTPUT_MODE IS _CONSOLE_OUTPUT
    viewer.hide();
#endif

    _shake(0);
}


void MetropolisAlgorithm::_shake(double h) {
    if(ic == Ferromagnetic){
        auto angle = 2*M_PI*RandUtils::random01();

        for (int k = 0; k < S.N; k++)
            S.set(k, angle);

    }
    else if(ic == Paramagnetic) {
        for (int k = 0; k < S.N; k++) {
            auto angle = 2*M_PI*RandUtils::random01();
            S.set(k, angle);
        }
    }
}


inline bool MetropolisAlgorithm::__shouldAccept(const ThermoUtils::Real deltaE) {
    if(deltaE<0) return true;

    const double r = RandUtils::random01();

    const double z = ThermoUtils::BoltzmannWeight(T, deltaE);

    return (r<z);
}


void MetropolisAlgorithm::MCStep() {
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

void MetropolisAlgorithm::__MCStepMetropolis() {
    _totalAcceptedSSCLastMCStep=0;

    if(sweeping == Random) {
        for (int ssf = 0; ssf < S.N; ++ssf) {
            // sorteio usando prob. (uniforme) do sitio estar na linha i:  P_i=1/L
            const int i = RandUtils::RandInt() % S.L;
            const int j = RandUtils::RandInt() % S.L;

            if(shouldOverrelax) {
                //auto e0 = S.e(i, j),
                //     e0_N = S.e(i, j-1);
                S.overrelax(i, j);
                //auto e1 = S.e(i, j),
                //     e1_N = S.e(i, j-1);
                //auto δ = e1 - e0,
                //     δ_N = e1_N - e0_N;
                //std::cout << δ << " " << δ_N << std::endl;
            }

            auto deltaTh = δ*(RandUtils::random01() - .5);
            const double deltaE = S.ssrDeltaE(i, j, h, deltaTh);

            if (__shouldAccept(deltaE)) {
                S.rotate(i, j, deltaTh);
                ++_totalAcceptedSSCLastMCStep;
            }
        }
    } else if(sweeping == Sequential) {
        for (int i = 0; i < S.L; ++i) {
            for (int j = 0; j < S.L; ++j) {
                auto deltaTh = δ*(RandUtils::random01() - .5);
                const double deltaE = S.ssrDeltaE(i, j, h, deltaTh);
                if (__shouldAccept(deltaE)) {
                    S.rotate(i, j, deltaE);
                    ++_totalAcceptedSSCLastMCStep;
                }
            }
        }
    }
}
void MetropolisAlgorithm::__MCStepKawasaki() {
    auto N = S.N;

    for (int ssf = 0; ssf < N; ++ssf) {
        const auto s1 = RandUtils::RandInt() % N; // sorteio usando prob. (uniforme) do sitio estar na linha i:  P_i=1/L
        const auto s2 = RandUtils::RandInt() % N;

        if(S.theta(s1) == S.theta(s2)) continue;

        const double deltaE = S.tseDeltaE(s1, s2, h);

        if (__shouldAccept(deltaE)) {
            throw "Kawasaki not implemented.";
            // S.rotate(s1);
            // S.rotate(s2);
        }
    }
}


void MetropolisAlgorithm::Simulate(int MCSteps, int transientSize) {
    _shake(h);

    std::cout.precision(8);
    const auto &N = S.N;
    const auto Nd = double(N);

#if OUTPUT_MODE IS _CONSOLE_OUTPUT
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

        ThermoOutput::SystemParams p = {T, h, δ, shouldRun, shouldOverrelax};
        ThermoOutput::OutputData data = {MCSteps, mcStep, transientSize, S, h,
                                         _totalAcceptedSSCLastMCStep, S.N-_totalAcceptedSSCLastMCStep};

        if (!vcOutput->doOperate(p, data)) exit(0);
    } // end sim for
#endif

}

void
MetropolisAlgorithm::_outputDataToConsole(const RealVector &e, const RealVector &e2, const RealVector &m,
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

void MetropolisAlgorithm::set_T(ThermoUtils::Real T) {
    this->T = T;
}

void MetropolisAlgorithm::set_h(ThermoUtils::Real h) {
    this->h = h;
}



