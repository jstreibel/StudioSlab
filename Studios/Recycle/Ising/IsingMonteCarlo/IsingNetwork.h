//
// Created by joao on 15/05/2021.
//

#ifndef ISING_ISINGNETWORK_H
#define ISING_ISINGNETWORK_H

#include <bitset>

#include "../COMPILE_CONFIG.h"

typedef std::bitset<MAX_BITSET_SIZE> StateType;

class IsingNetwork {
    StateType S;

public:
    const int L, N;
    IsingNetwork(int L);

    std::string NStates();

    void ImprimeCabecalho() const;
    void print(bool datFileSafe=true) const;

    int ijtok_periodic(int i, int j) const;
    int ijtok_abs(int i, int j) const;
    int s(int k) const;
    int s(int i, int j) const;
    double E(double h) const;
    double M() const;
    bool areNeighbors(int i1, int j1, int i2, int j2) const;

    void set(int k, bool value);

    /**
     * Calculates energy density at site i, j with h=0;
     * @param i
     * @param j
     * @return
     */
    int e(int i, int j) const;

    /**
     * Calculates a proposed single-spin-flip energy difference.
     * @param i
     * @param j
     * @param h
     * @return
     */
    double ssfDeltaE(int i, int j, double h) const;

    /**
     * Calculates a proposed two-spin exchange energy difference.
     * @param s1
     * @param s2
     * @param h
     * @return
     */
    double tseDeltaE(int s1, int s2, double h) const;

    void flip(int k);
    void flip(int i, int j);

    void operator = (StateType S);
    void operator = (unsigned long long  S);

};


#endif //ISING_ISINGNETWORK_H
