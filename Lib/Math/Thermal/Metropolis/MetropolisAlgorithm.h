//
// Created by joao on 15/05/2021.
//

#ifndef ISING_ISINGMONTECARLOCALCULATOR_H
#define ISING_ISINGMONTECARLOCALCULATOR_H

#include "../Model/XYNetwork.h"
#include "Math/Thermal/Utils/ThermoUtils.h"
#include "../IO/SingleSim/SingleSimViewController.h"

#include "Core/Backend/SFML-Nuklear/COMPILE_CONFIG.h"


typedef std::vector<Real> RealVector;


class MetropolisAlgorithm {

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

private:
    Real T;
    Real h;

    Real δ=2*M_PI;

    /** Total single-spin change accepted during last MCStep.  */
    int _totalAcceptedSSCLastMCStep=0;

    XYNetwork S;

    bool __shouldAccept(Real deltaE) ;


    void __MCStepMetropolis();
    void __MCStepKawasaki();

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
    explicit MetropolisAlgorithm(int L, Real T, Real h,
                                 ThermoOutput::ViewControlBase *viewer, InitialConditions ic,
                                 Dynamic dynamic, Sweeping sweeping);

    ~MetropolisAlgorithm() = default;

    void set_T(Real T);
    void set_h(Real h);
    void MCStep();
    void Simulate(int MCSteps, int transientSize);

    [[nodiscard]] const XYNetwork &getS() const {return S;}

private:
    ThermoOutput::ViewControlBase *vcOutput;

    static void
    _outputDataToConsole(const RealVector &e, const RealVector &e2, const RealVector &m, const RealVector &m2,
                         const RealVector &m4, long double T, double N);
};

#endif //ISING_ISINGMONTECARLOCALCULATOR_H
