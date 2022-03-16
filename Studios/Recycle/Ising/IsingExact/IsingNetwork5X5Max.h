//
// Created by joao on 15/05/2021.
//

#ifndef ISING_ISINGNETWORK5X5MAX_H
#define ISING_ISINGNETWORK5X5MAX_H

#include <iostream>
#include <iomanip>

typedef unsigned long IsingState;

class IsingNetwork_5x5max {
    IsingState S;

public:
    const int L;
    const int N;

    uint64_t NStates();

    IsingNetwork_5x5max(int L, IsingState S0=0);

    void ImprimeCabecalho() const;
    void print(bool datFileSafe=true) const;

    int ijtok_periodic(int i, int j) const;
    int ijtok_abs(int i, int j) const;
    int s(int k) const;
    int s(int i, int j) const;
    double E() const;
    double M() const;

    double ssfDeltaE(int i, int j) const;

    void flip(int i, int j);

    void operator = (const IsingState S);
    void operator = (const int S);
    bool operator < (const int N) const;
    IsingNetwork_5x5max& operator ++();
};


#endif //ISING_ISINGNETWORK5X5MAX_H
