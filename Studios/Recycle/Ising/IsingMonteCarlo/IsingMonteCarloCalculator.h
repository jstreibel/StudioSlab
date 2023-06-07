//
// Created by joao on 15/05/2021.
//

#ifndef ISING_ISINGMONTECARLOCALCULATOR_H
#define ISING_ISINGMONTECARLOCALCULATOR_H

#include "IsingNetwork.h"
#include "../Utils/ThermoUtils.h"
#include "../IO/SingleSim/SingleSimViewController.h"

#include "../COMPILE_CONFIG.h"

typedef std::vector<Real> RealVector;

class IsingMonteCarloCalculator {

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
private:
    Real T;
    Real h;

    IsingNetwork S;

    static bool __shouldAccept(Real deltaE) ;


    void __MCStepMetropolis();
    void __MCStepKawasaki();

    void _shake(double h);
public:

    explicit IsingMonteCarloCalculator(int L, Real T, Real h, ThermoOutput::ViewControlBase *viewer, InitialConditions ic, Dynamic dynamic, Sweeping sweeping);

    ~IsingMonteCarloCalculator() = default;

    void set_T(Real T);
    void set_h(Real h);
    void MCStep();
    void Simulate(int MCSteps, int transientSize);

    [[nodiscard]] const IsingNetwork &getS() const {return S;}

private:
    ThermoOutput::ViewControlBase *vcOutput;

    static void
    _outputDataToConsole(const RealVector &e, const RealVector &e2, const RealVector &m, const RealVector &m2,
                         const RealVector &m4, long double T, double N);
};

#endif //ISING_ISINGMONTECARLOCALCULATOR_H
