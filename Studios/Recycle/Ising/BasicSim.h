//
// Created by joao on 22/07/2021.
//

#ifndef ISING_BASICSIM_H
#define ISING_BASICSIM_H


#define ALWAYS_USE_MONTECARLO true


#include "IsingMonteCarlo/IsingMonteCarloCalculator.h"
#include "IsingExact/IsingExactCalculator.h"

#include <iostream>
#include <string>


int RunBasicSim(int argc, char **argv){
    int L=20;
    int TSteps = 75;
    int MCSteps = 5000;
    int transient;
    double T=1;

    bool simManyT = true;

    if(argc>1){
        std::string firstArg = argv[1];
        if(firstArg == "-h" || firstArg == "--help" || firstArg == "help" || firstArg == "ajuda" || firstArg == "socorro"){
            std::cout << "Ajuda. Todos os argumentos sao opcionais. \n"
                         "\n"
                         "\n"
                         "Utilizacao:  ./Ising L N_mc N_eq T\n"
                         "\n"
                         "Em detalhes: ./Ising <<L (int)>> <<N passos de Monte carlo (int)>> <<N passos para transiente (int)>> <<T (real)>>\n"
                         "\n"
                         "\n"
                         "EXEMPLO:\n"
                         "  ./Ising 50 5000 3500 2.45\n"
                         "\n"
                         "     Essa execucao roda uma rede LxL=50x50 em banho termico a temperatura T=2.45 (adimensional). Sao rodados\n"
                         " 5000 (cinco mil) passos de Montecarlo (ou unidades de tempo) e o transiente eh considerado como ocorrendo\n"
                         " ate o tempo (passo de Montecarlo) N_eq=3500.\n"
                         "\n";

            exit(0);
        }

        L = atoi(argv[1]);
    }
    if(argc>2){
        MCSteps = atoi(argv[2]);
    }
    if(argc>3) transient = atoi(argv[3]); else transient = int((6./7) * MCSteps);
    if(argc>4) {
        simManyT = false;
        T = atof(argv[4]);
    }

    std::cout << "# Ising Rede " << L << "x" << L << "=" << (L*L) << " sitios.\n";
    double Tmin = 1e-1, Tmax = 4.5, dT = (Tmax-Tmin) / TSteps;

    try {
        if (L < 6 && !ALWAYS_USE_MONTECARLO) {
            IsingNetwork_5x5max S(L);


            std::cout << "# Calculo exato (" << S.NStates() << " configuracoes da rede) da rede para valores de temperatura entre Tmin=" << dT << " ate Tmax=" << Tmax
                      << " em " << Tmax / dT << " passos de tamanho dT=" << dT << " cada.\n";

            S.ImprimeCabecalho();
            ExactIsingCalculator IsingCalc(L);
            for (double T = Tmin; T < Tmax; T += dT)
                IsingCalc.ComputeAll(T);

        }
        else {
            std::cout << "# Calculo de Montecarlo da rede para temperatura T=" << T << " e um total de t_max="
                      << MCSteps
                      << " passos de Montecarlo, com transiente em t_t="<<transient<<". \n";

            std::cout << "#1   2    3   4    5    6   7    8    9\n";
            std::cout << "#T   e   e2   m   m2   m4   Cv   Xi   B\n";

            auto *viewControl = new ThermoOutput::SingleSimViewController(L, MCSteps, transient);

            IsingMonteCarloCalculator mcCalculator(L, T, .0, viewControl,
                                                   IsingMonteCarloCalculator::Paramagnetic,
                                                   IsingMonteCarloCalculator::Metropolis,
                                                   IsingMonteCarloCalculator::Random);

            if(simManyT) for (double T = Tmin; T <= Tmax + .1 * dT; T += dT) {
                mcCalculator.set_T(T);
                mcCalculator.Simulate(MCSteps, transient);
            }
            else mcCalculator.Simulate(MCSteps, transient);

            std::cout << "\n\n";
        }
    } catch(const char* exception) {
        std::cout << "Ising exception: " << exception << "\n";
    }

    std::cout << std::endl;

    return 0;
}

#endif //ISING_BASICSIM_H
