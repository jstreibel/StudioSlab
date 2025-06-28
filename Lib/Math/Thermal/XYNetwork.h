//
// Created by joao on 15/05/2021.
//

#ifndef ISING_ISINGNETWORK_H
#define ISING_ISINGNETWORK_H

#include <bitset>
#include <vector>

#include "Utils/Types.h"

// #include "Core/Backend/SFML/COMPILE_CONFIG.h"
#include "ThermoUtils.h"


namespace Slab::Math {

    typedef Vector<DevFloat> StateType;

    class XYNetwork {
        StateType ThetaField;

    public:
        const int L, N;

        explicit XYNetwork(int L);

        int ij_to_k_periodic(int i, int j) const;

        int ij_to_k_abs(int i, int j) const;

        DevFloat theta(int k) const;

        DevFloat theta(int i, int j) const;

        DevFloat E(DevFloat h) const;

        DevFloat M() const;

        bool areNeighbors(int i1, int j1, int i2, int j2) const;

        void set(int k, DevFloat value);

        /**
         * Calculates energy density at site i, j with h=0;
         * @param i
         * @param j
         * @return
         */
        DevFloat e(int i, int j) const;

        /**
         * Calculates a proposed single-spin-rotate delta angle energy difference.
         * @param i
         * @param j
         * @param h
         * @return
         */
        DevFloat ssrDeltaE(int i, int j, DevFloat h, DevFloat delta) const;

        /**
         * Calculates a proposed single-spin-overrelaxation with rotation delta angle energy difference.
         * @param i
         * @param j
         * @param h
         * @return
         */
        DevFloat ssorrDeltaE(int i, int j, DevFloat h, DevFloat delta) const;

        void overrelax(int i, int j);

        /**
         * Calculates a proposed two-spin exchange energy difference.
         * @param s1
         * @param s2
         * @param h
         * @return
         */
        double tseDeltaE(int s1, int s2, double h) const;

        void rotate(int k, DevFloat angle);

        void rotate(int i, int j, DevFloat angle);

        void operator=(StateType S);
        //void operator = (const Vector<long double> S);

    };


}

#endif //ISING_ISINGNETWORK_H
