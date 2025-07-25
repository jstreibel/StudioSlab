//
// Created by joao on 7/23/24.
//

#include "NonlinearKGPotential.h"
#include "NullFunction.h"

#include <cmath>

namespace Slab::Math::RtoR {
#ifdef HAS_QUADMATH
    auto log_gamma = lgammaq;
#else
    auto log_gamma = lgamma;
#endif

    Real128 lambda(int n, int N) {
        fix arg4 = DevFloat(N-n)/2.+1.;
        fix arg5 = DevFloat(N+n)/2.+1.;

        fix t1 = n*log(2.);
        fix t2 = log(2./(M_PI*(DevFloat)n));
        fix t3 = -log_gamma(n+1);
        fix t4 = -log_gamma(arg4);
        fix t5 =  log_gamma(arg5);

        fix exponent = t1+t2+t3+t4+t5;

        // TODO: make exp128 work with Intel compilers
        // return expf128(exponent);
        return exp(exponent);
    }

    VPrime::VPrime
    (DevFloat A, int N, DevFloat s)
    : FunctionT(nullptr, false)
    , coeffs(CalcCoefficients(N, s))
    , N(N), A(A), s(s)
    { }

    DevFloat VPrime::getA() const {
        return A;
    }

    void VPrime::setA(DevFloat new_A) { this->A = new_A; }

    DevFloat VPrime::operator()(DevFloat phi) const {
        DevFloat value=0;

        for(auto n=1; n<=N; n+=2) {
            fix x = phi/A;
            value += get_lambda(n) * odd_power(x, n);
        }

        return value;
    }

    FRealVector VPrime::CalcCoefficients(int N, DevFloat s) {
        Vector<DevFloat> coeffs;

        if(!(N%2)) throw Exception(Str(__PRETTY_FUNCTION__) + " works with odd-valued N.");
        if  (N<0)  throw Exception(Str(__PRETTY_FUNCTION__) + " works with positive-valued N.");

        for(auto n=1; n<=N; n+=2) {
            fix i=(n-1)/2;

            fix sign = 1.- (i%2)*2.;

            fix coef = sign * lambda(n, N) / pow((n+1), s);

            coeffs.emplace_back(coef);
        }

        return coeffs;
    }

    CountType VPrime::getN() const { return N; }

    auto VPrime::setN(CountType new_N) -> void {
        this->N = new_N;
        coeffs = CalcCoefficients((int)N, s);
    }

    DevFloat VPrime::get_s() const {
        return s;
    }

    void VPrime::set_s(DevFloat new_s) {
        this->s = new_s;
        coeffs = CalcCoefficients((int)N, s);
    }

    DevFloat VPrime::even_power(const DevFloat &x, const int &unchecked_even_n) {
        auto n = unchecked_even_n;

        auto result = 1.0;
        while (n > 1) {
            result *= x*x; // Square the base
            n /= 2; // Divide n by 2
        }
        return result;
    }

    DevFloat VPrime::odd_power(const DevFloat &x, const int &unchecked_odd_n) {
        auto n = unchecked_odd_n-1;

        auto result = x;
        while (n --> 0) result *= x;

        return result;
    }


    DevFloat NonlinearKGPotential::operator()(DevFloat phi) const { return VPrime::operator()(phi) * phi; }

    NonlinearKGPotential::NonlinearKGPotential(DevFloat A, CountType N, DevFloat s)
    : VPrime(A, (int)N, s) {    }

    TPointer<Base::FunctionT<DevFloat, DevFloat>::Type> NonlinearKGPotential::diff(int n) const {
        if(n!=0) return New<RtoR::NullFunction>();

        // The _actual_ derivative (not as \phi^{-1}V(\phi))
        return New<VPrime>(getA(), getN(), get_s()-1);
    }
} // Slab::Math::RtoR