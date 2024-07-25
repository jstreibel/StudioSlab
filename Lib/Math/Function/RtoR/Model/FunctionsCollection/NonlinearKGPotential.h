//
// Created by joao on 7/23/24.
//

#ifndef STUDIOSLAB_NONLINEARKGPOTENTIAL_H
#define STUDIOSLAB_NONLINEARKGPOTENTIAL_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    Real128 lambda(int n, int N);

    class VPrime : public RtoR::Function {
        RealVector coeffs;
        Count N;
        Real A;
        Real s;

        inline Real get_lambda(int n) const {
            return coeffs[(n-1)/2];
        }

        [[maybe_unused]] static inline Real
        even_power(const Real &x, const int &unchecked_even_n);
        static inline Real
        odd_power(const Real &x, const int &unchecked_odd_n);
    public:
        VPrime(Real A, int N, Real s);

        Real operator()(Real phi) const override;

        Real getA() const;
        void setA(Real);

        Real get_s() const;
        void set_s(Real);

        auto getN() const -> Count;
        auto setN(Count N) -> void;


        static RealVector
        CalcCoefficients(int N, Real s);
    };

    class NonlinearKGPotential : public VPrime {
    public:
        NonlinearKGPotential(Real A, Count N, Real s);
        Real operator()(Real phi) const override;

        Pointer<Type> diff(int n) const override;
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_NONLINEARKGPOTENTIAL_H
