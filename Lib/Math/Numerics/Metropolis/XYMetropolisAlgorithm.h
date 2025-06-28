//
// Created by joao on 15/05/2021.
//

#ifndef ISING_ISINGMONTECARLOCALCULATOR_H
#define ISING_ISINGMONTECARLOCALCULATOR_H

#include "Math/Thermal/XYNetwork.h"
// #include "Core/Backend/SFML/COMPILE_CONFIG.h"
#include "Math/Thermal/InputOutput.h"


namespace Slab::Math {

    class XYMetropolisAlgorithm {
    public:

        enum InitialConditions {
            Ferromagnetic,
            Paramagnetic
        };

        enum Dynamic {
            Metropolis,
            Kawasaki
        };

        enum Sweeping {
            Random,
            Sequential
        };

        InitialConditions ic;
        Dynamic aDynamic;
        Sweeping sweeping;

        bool shouldOverrelax = false;
        bool shouldRun = true;

    private:
        DevFloat T;
        DevFloat h;

        DevFloat δ = 2 * M_PI;

        /** Total single-spin change accepted during last MCStep.  */
        int _totalAcceptedSSCLastMCStep = 0;

        XYNetwork S;

        bool shouldAccept(DevFloat deltaE) const;


        void MCStepMetropolis();

        void MCStepKawasaki();

        void _shake(double h);

    public:

        /**
         *
         * @param L
         * @param T
         * @param h external magnetic field (not implemented)
         * @param viewer
         * @param ic
         * @param dynamic
         * @param sweeping
         */
        explicit XYMetropolisAlgorithm(int L, DevFloat T, DevFloat h,
                                       InitialConditions ic,
                                       Dynamic dynamic,
                                       Sweeping sweeping);

        ~XYMetropolisAlgorithm() = default;

        void set_T(DevFloat T);

        void set_h(DevFloat h);

        void MCStep();

        auto getParams() -> Lost::ThermoOutput::SystemParams;

        auto getData() -> Lost::ThermoOutput::OutputData;

        const XYNetwork &getS() const { return S; }

    };


}

#endif //ISING_ISINGMONTECARLOCALCULATOR_H
