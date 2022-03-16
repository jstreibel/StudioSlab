//
// Created by joao on 22/07/2021.
//

#ifndef ISING_MANYHISTORIES_H
#define ISING_MANYHISTORIES_H


#include "IsingMonteCarlo/IsingMonteCarloCalculator.h"
#include "IO/ManyHistories/HistoryViewController.h"

#include <iostream>
#include <string>


int RunManyHistories(int argc, char **argv){
    try {
        if(argc != 6) throw "Wrong argc. Use ./Ising [sqrtNSims] [L] [MCSteps] [tau_eq] [T]";

        auto arg = 0;

        int sqrtNSims = atoi(argv[++arg]);
        int L = atoi(argv[++arg]);
        int MCSteps = atoi(argv[++arg]);
        int tau_eq = atoi(argv[++arg]);
        double T = atof(argv[++arg]);


        auto *vc = new ThermoOutput::HistoryViewController(T, L, MCSteps, tau_eq, sqrtNSims);

        std::cout << "# Running many histories @ T=" << T << std::endl;
        vc->run();

        std::cout << "\n\n";
    } catch(const char* exception) {
        std::cout << "Ising exception: " << exception << "\n";
    }


    std::cout << std::endl;

    return 0;
}


#endif //ISING_MANYHISTORIES_H
