//
// Created by joao on 15/05/2021.
//

#include "RandUtils.h"

#include <iostream>
#include <random>

namespace Slab::RandUtils {
    std::mt19937 mt_uniform_real(1);
    std::mt19937 mt_uniform_uint(1);
    std::mt19937 mt_gaussian_noise(1);

    std::uniform_real_distribution<Real> UniformRealRandGen(0, 1);
    std::uniform_int_distribution<unsigned int> UniformUIntRandGen(0, INT32_MAX);

    void SeedUniformReal  (int s) { mt_uniform_real  .seed(s); }
    void SeedUniformUInt  (int s) { mt_uniform_uint  .seed(s); }
    void SeedGaussianNoise(int s) { mt_gaussian_noise.seed(s); }

    Real RandomUniformReal01() { return UniformRealRandGen(mt_uniform_real); }

    Real RandomUniformReal(Real a, Real b){ return (b - a) * RandomUniformReal01() + a; }

    unsigned RandomUniformUInt() { return UniformUIntRandGen(mt_uniform_uint); }

    double GaussianNoise(double mean, double standard_deviation) {
        std::normal_distribution<double> distribution(mean, standard_deviation);
        return distribution(mt_gaussian_noise);
    }


    // Function to solve for sigma using numerical methods
    double solve_for_sigma(double mode, double std_dev) {
        double left = 0.1, right = 5.0; // Reasonable initial range for sigma
        double tol = 1e-6; // Tolerance for convergence
        double sigma = 0;

        while (right - left > tol) {
            sigma = (left + right) / 2;
            double u = exp(sigma * sigma);
            double lhs = (std_dev / mode) * (std_dev / mode);
            double rhs = u * u * u * (u - 1);
            if (lhs < rhs) {
                right = sigma;
            } else {
                left = sigma;
            }
        }

        return sigma;
    }

// Function to calculate mu and sigma
    void calculate_parameters(double x0, double x_std, double& mu, double& sigma) {
        sigma = solve_for_sigma(x0, x_std);
        mu = log(x0) + sigma * sigma;
    }

// Function to generate log-normal distribution values
    std::vector<double> GenerateLognormalValues(int n, double x0, double x_std, int seed) {

        double mu, sigma;
        calculate_parameters(x0, x_std, mu, sigma);

        std::mt19937 gen(seed);
        std::lognormal_distribution<> d(mu, sigma);

        std::vector<double> values(n);
        for (int i = 0; i < n; ++i) {
            values[i] = d(gen);
        }

        return values;
    }

}


namespace Slab::LostAndFound {
    void imprimeBinario(uint64_t a) {
        for (int i = 63; i >= 0; --i) {
            uint64_t val = (a & (1 << i)) >> i;
            std::cout << val;
        }
    }
}