//
// Created by joao on 15/05/2021.
//

#ifndef ISING_ISINGMONTECARLOCALCULATOR_H
#define ISING_ISINGMONTECARLOCALCULATOR_H

#include "../Model/XYNetwork.h"
#include "Phys/Thermal/Utils/ThermoUtils.h"
#include "../IO/SingleSim/SingleSimViewController.h"

#include "Studios/Backend/SFML-Nuklear/COMPILE_CONFIG.h"


typedef std::vector<ThermoUtils::Real> RealVector;


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
    ThermoUtils::Real T;
    ThermoUtils::Real h;

    ThermoUtils::Real δ=2*M_PI;

    /** Total single-spin change accepted during last MCStep.  */
    int _totalAcceptedSSCLastMCStep=0;

    XYNetwork S;

    bool __shouldAccept(ThermoUtils::Real deltaE) ;


    void __MCStepMetropolis();
    void __MCStepKawasaki();

    void _shake(double h);
public:

    explicit MetropolisAlgorithm(int L, ThermoUtils::Real T, ThermoUtils::Real h,
                                 ThermoOutput::ViewControlBase *viewer, InitialConditions ic,
                                 Dynamic dynamic, Sweeping sweeping);

    ~MetropolisAlgorithm() = default;

    void set_T(ThermoUtils::Real T);
    void set_h(ThermoUtils::Real h);
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
