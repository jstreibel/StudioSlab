//
// Created by joao on 22/07/2021.
//

#ifndef ISING_BASICSIM_H
#define ISING_BASICSIM_H


#include <Math/Thermal/Metropolis/MetropolisAlgorithm.h>

#include <string>
#include <iostream>


int RunBasicSim(int argc, const char **argv){
    int L=20;
    int TSteps = 75;
    int MCSteps = 5000;
    int transient;
    double T=1;

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
        T = atof(argv[4]);
    }

    std::cout << "# Ising Rede " << L << "x" << L << "=" << (L*L) << " sitios.\n";
    double Tmin = 1e-1, Tmax = 4.5, dT = (Tmax-Tmin) / TSteps;

    try {
            std::cout << "# Calculo de Montecarlo da rede para temperatura T=" << T << " e um total de t_max="
                      << MCSteps
                      << " passos de Montecarlo, com transiente em t_t="<<transient<<". \n";

            std::cout << "#1   2    3   4    5    6   7    8    9\n";
            std::cout << "#T   e   e2   m   m2   m4   Cv   Xi   B\n";

            auto *viewControl = new ThermoOutput::SingleSimViewController(L, MCSteps, transient);

            MetropolisAlgorithm mcCalculator(L, T, .0, viewControl,
                                                   MetropolisAlgorithm::Ferromagnetic,
                                                   MetropolisAlgorithm::Metropolis,
                                                   MetropolisAlgorithm::Random);

            mcCalculator.Simulate(MCSteps, transient);

            std::cout << "\n\n";
    } catch(const char* exception) {
        std::cout << "Ising exception: " << exception << "\n";
    }

    std::cout << std::endl;

    return 0;
}

#endif //ISING_BASICSIM_H
