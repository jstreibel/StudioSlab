//
// Created by joao on 14/05/2021.
//

#ifndef ISING_ISINGEXACTCALCULATOR_H
#define ISING_ISINGEXACTCALCULATOR_H

#include "IsingNetwork5X5Max.h"
#include "../Utils/RandUtils.h"

#include <stdint-gcc.h>
#include <iostream>
#include <bitset>
#include <iomanip>
#include <cmath>


class ExactIsingCalculator {
    IsingNetwork_5x5max S;

public:
    explicit ExactIsingCalculator(int L);

    void ComputeAll(double T);
};

#endif //ISING_ISINGEXACTCALCULATOR_H
