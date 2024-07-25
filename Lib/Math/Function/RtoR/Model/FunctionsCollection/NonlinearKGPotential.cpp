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
        fix arg4 = Real(N-n)/2.+1.;
        fix arg5 = Real(N+n)/2.+1.;

        fix t1 = n*log(2.);
        fix t2 = log(2./(M_PI*(Real)n));
        fix t3 = -log_gamma(n+1);
        fix t4 = -log_gamma(arg4);
        fix t5 =  log_gamma(arg5);

        fix exponent = t1+t2+t3+t4+t5;

        return expf128(exponent);
    }

    VPrime::VPrime
    (Real A, int N, Real s)
    : FunctionT(nullptr, false)
    , coeffs(CalcCoefficients(N, s))
    , N(N), A(A), s(s)
    { }

    Real VPrime::getA() const {
        return A;
    }

    void VPrime::setA(Real new_A) { this->A = new_A; }

    Real VPrime::operator()(Real phi) const {
        Real value=0;

        for(auto n=1; n<=N; n+=2) {
            fix x = phi/A;
            value += get_lambda(n) * odd_power(x, n);
        }

        return value;
    }

    RealVector VPrime::CalcCoefficients(int N, Real s) {
        Vector<Real> coeffs;

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

    Count VPrime::getN() const { return N; }

    auto VPrime::setN(Count new_N) -> void {
        this->N = new_N;
        coeffs = CalcCoefficients((int)N, s);
    }

    Real VPrime::get_s() const {
        return s;
    }

    void VPrime::set_s(Real new_s) {
        this->s = new_s;
        coeffs = CalcCoefficients((int)N, s);
    }

    Real VPrime::even_power(const Real &x, const int &unchecked_even_n) {
        auto n = unchecked_even_n;

        auto result = 1.0;
        while (n > 1) {
            result *= x*x; // Square the base
            n /= 2; // Divide n by 2
        }
        return result;
    }

    Real VPrime::odd_power(const Real &x, const int &unchecked_odd_n) {
        auto n = unchecked_odd_n-1;

        auto result = x;
        while (n --> 0) result *= x;

        return result;
    }


    Real NonlinearKGPotential::operator()(Real phi) const { return VPrime::operator()(phi) * phi; }

    NonlinearKGPotential::NonlinearKGPotential(Real A, Count N, Real s)
    : VPrime(A, (int)N, s) {    }

    Pointer<Base::FunctionT<Real, Real>::Type> NonlinearKGPotential::diff(int n) const {
        if(n!=0) return New<RtoR::NullFunction>();

        // The _actual_ derivative (not as \phi^{-1}V(\phi))
        return New<VPrime>(getA(), getN(), get_s()-1);
    }
} // Slab::Math::RtoR