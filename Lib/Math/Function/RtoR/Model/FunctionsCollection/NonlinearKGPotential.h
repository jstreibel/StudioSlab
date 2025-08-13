//
// Created by joao on 7/23/24.
//

#ifndef STUDIOSLAB_NONLINEARKGPOTENTIAL_H
#define STUDIOSLAB_NONLINEARKGPOTENTIAL_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    Real128 lambda(int n, int N);

    class VPrime : public RtoR::Function {
        FRealVector coeffs;
        CountType N;
        DevFloat A;
        DevFloat s;

        inline DevFloat get_lambda(int n) const {
            return coeffs[(n-1)/2];
        }

        [[maybe_unused]] static inline DevFloat
        even_power(const DevFloat &x, const int &unchecked_even_n);
        static inline DevFloat
        odd_power(const DevFloat &x, const int &unchecked_odd_n);
    public:
        VPrime(DevFloat A, int N, DevFloat s);

        DevFloat operator()(DevFloat phi) const override;

        DevFloat getA() const;
        void setA(DevFloat);

        DevFloat get_s() const;
        void set_s(DevFloat);

        auto getN() const -> CountType;
        auto setN(CountType N) -> void;


        static FRealVector
        CalcCoefficients(int N, DevFloat s);
    };

    class NonlinearKGPotential : public VPrime {
    public:
        NonlinearKGPotential(DevFloat A, CountType N, DevFloat s);
        DevFloat operator()(DevFloat phi) const override;

        TPointer<Type> diff(int n) const override;
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_NONLINEARKGPOTENTIAL_H
