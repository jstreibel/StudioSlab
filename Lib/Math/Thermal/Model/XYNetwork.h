//
// Created by joao on 15/05/2021.
//

#ifndef ISING_ISINGNETWORK_H
#define ISING_ISINGNETWORK_H

#include <bitset>
#include <vector>

#include "Utils/Types.h"

#include "Core/Backend/SFML-Nuklear/COMPILE_CONFIG.h"
#include "Math/Thermal/Utils/ThermoUtils.h"

typedef std::vector<Real> StateType;

class XYNetwork {
    StateType ThetaField;

public:
    const int L, N;
    XYNetwork(int L);

    int ij_to_k_periodic(int i, int j) const;
    int ij_to_k_abs(int i, int j) const;
    Real theta(int k) const;
    Real theta(int i, int j) const;
    Real E(Real h) const;
    Real M() const;
    bool areNeighbors(int i1, int j1, int i2, int j2) const;

    void set(int k, Real value);

    /**
     * Calculates energy density at site i, j with h=0;
     * @param i
     * @param j
     * @return
     */
    Real e(int i, int j) const;

    /**
     * Calculates a proposed single-spin-rotate delta angle energy difference.
     * @param i
     * @param j
     * @param h
     * @return
     */
    Real ssrDeltaE(int i, int j, Real h, Real delta) const;

    /**
     * Calculates a proposed single-spin-overrelaxation with rotation delta angle energy difference.
     * @param i
     * @param j
     * @param h
     * @return
     */
    Real ssorrDeltaE(int i, int j, Real h, Real delta) const;

    void overrelax(int i, int j);

    /**
     * Calculates a proposed two-spin exchange energy difference.
     * @param s1
     * @param s2
     * @param h
     * @return
     */
    double tseDeltaE(int s1, int s2, double h) const;

    void rotate(int k, Real angle);
    void rotate(int i, int j, Real angle);

    void operator = (StateType S);
    //void operator = (const std::vector<long double> S);

};


#endif //ISING_ISINGNETWORK_H
