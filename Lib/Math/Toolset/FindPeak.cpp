//
// Created by joao on 7/7/24.
//

#include "FindPeak.h"

#include <cmath>

namespace Slab::Math {

    // Function to interpolate using Lagrange's method
    Real LagrangeInterpolate(const RealVector& x, const RealVector& y, Real t, int order) {
        int n = x.size();
        Real result = 0.0;
        for (int i = 0; i < order + 1; ++i) {
            Real term = y[i];
            for (int j = 0; j < order + 1; ++j) {
                if (i != j) {
                    term *= (t - x[j]) / (x[i] - x[j]);
                }
            }
            result += term;
        }
        return result;
    }

    // Derivative of the Lagrange polynomial
    Real LagrangeDerivative(const RealVector& x, const RealVector& y, Real t, int order) {
        int n = x.size();
        Real result = 0.0;
        for (int i = 0; i < order + 1; ++i) {
            Real term = y[i];
            Real sum = 0.0;
            for (int j = 0; j < order + 1; ++j) {
                if (i != j) {
                    Real prod = 1.0;
                    for (int k = 0; k < order + 1; ++k) {
                        if (k != i && k != j) {
                            prod *= (t - x[k]) / (x[i] - x[k]);
                        }
                    }
                    sum += prod / (x[i] - x[j]);
                }
            }
            result += term * sum;
        }
        return result;
    }

    // Function to find the peak using Newton's method
    Real FindPeak(const RealVector& x, const RealVector& y, Real initial_guess, int order) {
        Real t = initial_guess;
        for (int i = 0; i < 100; ++i) {
            Real f = LagrangeInterpolate(x, y, t, order);
            Real f_prime = LagrangeDerivative(x, y, t, order);
            Real t_new = t - f / f_prime;
            if (std::fabs(t_new - t) < 1e-6) break;
            t = t_new;
        }
        return t;
    }
}