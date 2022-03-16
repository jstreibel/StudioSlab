//
// Created by joao on 09/08/2021.
//

#ifndef ISING_MANYHISTORIESMANYTEMPERATURES_H
#define ISING_MANYHISTORIESMANYTEMPERATURES_H


#include "IsingMonteCarlo/IsingMonteCarloCalculator.h"
#include "IO/ManyHistories/QuenchViewController.h"

#include <iostream>
#include <string>


int RunManyHistoriesManyTemperatures(int argc, char **argv){
    try {
        if(argc != 6) throw "Wrong argc. Use ./Ising [sqrtNSims] [L] [tau_eq] [tau_corr] [total_measures]";

        auto arg = 0;

        int sqrtNSims = atoi(argv[++arg]);
        int L =         atoi(argv[++arg]);
        int tau_eq =    atoi(argv[++arg]);
        int tau_corr =  atoi(argv[++arg]);
        int total_measures = atoi(argv[++arg]);


        auto *vc = new ThermoOutput::QuenchViewController(L, tau_eq, tau_corr, total_measures, sqrtNSims);

        std::cout << "# Running quench of many histories" << std::endl;
        vc->run();

        std::cout << "\n\n";
    } catch(const char* exception) {
        std::cout << "Ising exception: " << exception << "\n";
    }


    std::cout << std::endl;

    return 0;
}


#endif //ISING_MANYHISTORIESMANYTEMPERATURES_H
