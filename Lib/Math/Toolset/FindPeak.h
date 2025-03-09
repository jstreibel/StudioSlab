//
// Created by joao on 7/7/24.
//

#ifndef STUDIOSLAB_FINDPEAK_H
#define STUDIOSLAB_FINDPEAK_H

#include "Utils/Numbers.h"
#include "Utils/Arrays.h"

namespace Slab::Math {
    inline double avgPeakPosition(const std::valarray<double>& y_vec, const size_t idx, const double x_min, const double dx, int n) {
        n = std::min(n, 2*static_cast<int>(idx)); // n tq ser <= 2*idx &&

        fix odd = !(n%2);

        const int i_start = -n/2;
        const int i_end = n/2;

        auto x_avg = .0;
        auto y_avg = .0;
        for(auto i=i_start; i<=i_end; ++i) {
            if(odd && i==0) continue;
            fix y_i = y_vec[idx + i];
            fix x_i = x_min + dx*(static_cast<Real>(idx) + static_cast<Real>(i));
            x_avg += x_i*y_i;
            y_avg += y_i;
        }

        return x_avg / y_avg;
    }

    inline double parabolicPeakPosition(const std::valarray<double>& y_vec, size_t idx, double x_min, double dx) {
        if (idx == 0 || idx >= y_vec.size() - 1) {
            throw std::invalid_argument("Index out of range for parabolic interpolation.");
        }

        double y_m1 = y_vec[idx - 1];
        double y_0 = y_vec[idx];
        double y_p1 = y_vec[idx + 1];

        double denominator = 2 * (y_m1 - 2 * y_0 + y_p1);
        if (denominator == 0) {
            throw std::runtime_error("Denominator in parabolic interpolation is zero.");
        }

        double peak_offset = (y_m1 - y_p1) / denominator; // Changed sign here
        double x_peak = x_min + (idx + peak_offset) * dx;

        return x_peak;
    }

    inline double cubicPeakPosition(const std::valarray<double>& y_vec, size_t idx, double x_min, double dx) {
        if (idx < 1 || idx >= y_vec.size() - 2) {
            throw std::invalid_argument("Index out of range for cubic interpolation.");
        }

        // Values at neighboring points
        double y_m1 = y_vec[idx - 1];
        double y_0 = y_vec[idx];
        double y_p1 = y_vec[idx + 1];
        double y_p2 = y_vec[idx + 2];

        // Construct the system of equations
        // A * coeffs = Y, where A is the matrix of x values and coeffs is [a, b, c, d]
        auto idxd = (Real)idx;
        double A[4][4] = {
                {pow(idxd-1, 3), pow(idxd-1, 2), idxd-1, 1},
                {pow(idxd, 3),   pow(idxd, 2),   idxd,   1},
                {pow(idxd+1, 3), pow(idxd+1, 2), idxd+1, 1},
                {pow(idxd+2, 3), pow(idxd+2, 2), idxd+2, 1}
        };
        double Y[4] = {y_m1, y_0, y_p1, y_p2};
        double coeffs[4];

        // Solve the system of equations using Gaussian elimination or any other method
        for (int i = 0; i < 4; ++i) {
            for (int j = i+1; j < 4; ++j) {
                if (abs(A[i][i]) < abs(A[j][i])) {
                    std::swap(A[i], A[j]);
                    std::swap(Y[i], Y[j]);
                }
            }
            for (int j = i+1; j < 4; ++j) {
                double factor = A[j][i] / A[i][i];
                for (int k = i; k < 4; ++k) {
                    A[j][k] -= factor * A[i][k];
                }
                Y[j] -= factor * Y[i];
            }
        }
        for (int i = 3; i >= 0; --i) {
            coeffs[i] = Y[i];
            for (int j = i+1; j < 4; ++j) {
                coeffs[i] -= A[i][j] * coeffs[j];
            }
            coeffs[i] /= A[i][i];
        }

        // Derivative of the cubic polynomial: 3ax^2 + 2bx + c = 0
        double a = coeffs[0];
        double b = coeffs[1];
        double c = coeffs[2];

        // Solve the quadratic equation for the derivative
        double discriminant = b*b - 3*a*c;
        if (discriminant < 0) {
            throw std::runtime_error("Discriminant is negative, no real roots.");
        }

        double x1 = (-b + sqrt(discriminant)) / (3*a);
        double x2 = (-b - sqrt(discriminant)) / (3*a);

        // Choose the root that is within the interval [idx, idx+1]
        double x_peak = (x1 >= idx && x1 <= idx+1) ? x1 : x2;
        // double x_peak = x2;
        double x_approx = x_min + x_peak * dx;

        return x_approx;
    }


    inline double nthOrderPeakPosition(const std::valarray<double>& y_vec, size_t idx, double x_min, double dx, int n) {
        n = std::min(n, 2*(int)idx); // n tq ser <= 2*idx &&

        if (idx >= y_vec.size() - n / 2) {
            throw std::invalid_argument("Index out of range for n-th order interpolation.");
        }

        // Initial guess for the peak position
        double x_peak = x_min + idx * dx;

        // Collect n+1 points around the peak
        std::vector<double> x_vals;
        std::vector<double> y_vals;
        for (int i = -n/2; i <= n/2; ++i) {
            x_vals.push_back(x_min + (idx + i) * dx);
            y_vals.push_back(y_vec[idx + i]);
        }

        // Compute the Lagrange interpolation polynomial
        double x_peak_new = 0.0;
        for (int i = 0; i <= n; ++i) {
            double L = 1.0;
            for (int j = 0; j <= n; ++j) {
                if (i != j) {
                    L *= (x_peak - x_vals[j]) / (x_vals[i] - x_vals[j]);
                }
            }
            x_peak_new += L * x_vals[i];
        }

        return x_peak_new;
    }



}

#endif //STUDIOSLAB_FINDPEAK_H
