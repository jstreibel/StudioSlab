//
// Created by joao on 15/05/2021.
//

#include "XYMetropolisAlgorithm.h"

#include "Utils/RandUtils.h"

#include <cmath>
#include <numeric>

#define IS ==


namespace Slab::Math {

    DevFloat avg(const FRealVector &v) {
        const DevFloat zero = 0.0;
        const DevFloat N = v.size();
        return std::accumulate(v.begin(), v.end(), zero) / N;
    }


    XYMetropolisAlgorithm::XYMetropolisAlgorithm(
            int L,
            DevFloat T,
            DevFloat h,
            InitialConditions ic,
            Dynamic dynamic,
            Sweeping sweeping)
            : S(L), T(T), h(h), ic(ic), aDynamic(dynamic), sweeping(sweeping) {
        _shake(0);
    }


    void XYMetropolisAlgorithm::_shake(double h) {
        if (ic == Ferromagnetic) {
            auto angle = 2 * M_PI * RandUtils::RandomUniformReal01();

            for (int k = 0; k < S.N; k++)
                S.set(k, angle);

        } else if (ic == Paramagnetic) {
            for (int k = 0; k < S.N; k++) {
                auto angle = 2 * M_PI * RandUtils::RandomUniformReal01();
                S.set(k, angle);
            }
        }
    }


    inline bool XYMetropolisAlgorithm::shouldAccept(const DevFloat deltaE) const {
        if (deltaE < 0) return true;

        const double r = RandUtils::RandomUniformReal01();

        const double z = ThermoUtils::BoltzmannWeight(T, deltaE);

        return (r < z);
    }


    void XYMetropolisAlgorithm::MCStep() {
        switch (aDynamic) {
            case Metropolis:
                MCStepMetropolis();
                break;
            case Kawasaki:
                MCStepKawasaki();
                break;
        }
    }

    void XYMetropolisAlgorithm::MCStepMetropolis() {
        _totalAcceptedSSCLastMCStep = 0;

        if (sweeping == Random) {
            for (int ssf = 0; ssf < S.N; ++ssf) {
                // sorteio usando prob. (uniforme) do sitio estar na linha i:  P_i=1/L
                const int i = (int) RandUtils::RandomUniformUInt() % S.L;
                const int j = (int) RandUtils::RandomUniformUInt() % S.L;

                if (shouldOverrelax) {
                    //auto e0 = S.e(i, j),
                    //     e0_N = S.e(i, j-1);
                    S.overrelax(i, j);
                    //auto e1 = S.e(i, j),
                    //     e1_N = S.e(i, j-1);
                    //auto δ = e1 - e0,
                    //     δ_N = e1_N - e0_N;
                    //std::cout << δ << " " << δ_N << std::endl;
                }

                auto deltaTh = δ * (RandUtils::RandomUniformReal01() - .5);
                const double deltaE = S.ssrDeltaE(i, j, h, deltaTh);

                if (shouldAccept(deltaE)) {
                    S.rotate(i, j, deltaTh);
                    ++_totalAcceptedSSCLastMCStep;
                }
            }
        } else if (sweeping == Sequential) {
            for (int i = 0; i < S.L; ++i) {
                for (int j = 0; j < S.L; ++j) {
                    auto deltaTh = δ * (RandUtils::RandomUniformReal01() - .5);
                    const double deltaE = S.ssrDeltaE(i, j, h, deltaTh);
                    if (shouldAccept(deltaE)) {
                        S.rotate(i, j, deltaE);
                        ++_totalAcceptedSSCLastMCStep;
                    }
                }
            }
        }
    }

    void XYMetropolisAlgorithm::MCStepKawasaki() {
        auto N = S.N;

        for (int ssf = 0; ssf < N; ++ssf) {
            const auto s1 =
                    RandUtils::RandomUniformUInt() % N; // sorteio usando prob. (uniforme) do sitio estar na linha i:  P_i=1/L
            const auto s2 = RandUtils::RandomUniformUInt() % N;

            if (S.theta((int) s1) == S.theta((int) s2)) continue;

            const double deltaE = S.tseDeltaE((int) s1, (int) s2, h);

            if (shouldAccept(deltaE)) {
                throw NotImplementedException("Kawasaki step method");
                // S.rotate(s1);
                // S.rotate(s2);
            }
        }
    }


    void XYMetropolisAlgorithm::set_T(DevFloat T) {
        this->T = T;
    }

    void XYMetropolisAlgorithm::set_h(DevFloat h) {
        this->h = h;
    }

    Lost::ThermoOutput::SystemParams XYMetropolisAlgorithm::getParams() {
        return {T, h, δ, shouldRun, shouldOverrelax};
    }

    auto XYMetropolisAlgorithm::getData() -> Lost::ThermoOutput::OutputData {
        return {S, h, _totalAcceptedSSCLastMCStep, S.N - _totalAcceptedSSCLastMCStep};
    }


}