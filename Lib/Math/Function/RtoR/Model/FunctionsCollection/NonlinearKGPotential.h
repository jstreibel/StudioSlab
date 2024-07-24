//
// Created by joao on 7/23/24.
//

#ifndef STUDIOSLAB_NONLINEARKGPOTENTIAL_H
#define STUDIOSLAB_NONLINEARKGPOTENTIAL_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    Real128 lambda(int n, int N);

    template<typename RealType = Real>
    class NonlinearKGPotential : public RtoR::Function {
        Vector<RealType> coeffs;
        Count N;
        RealType A;

        inline RealType get_lambda(int n) const {
            return coeffs[(n-1)/2];
        }
        static inline RealType
        even_power(const RealType &x, const int &unchecked_even_n) {
            auto n = unchecked_even_n;

            auto result = 1.0;
            while (n > 1) {
                result *= x*x; // Square the base
                n /= 2; // Divide n by 2
            }
            return result;
        }
    public:
        NonlinearKGPotential(Real A, int N)
        : FunctionT(nullptr, false)
        , coeffs(CalcCoefficients(N))
        , N(N), A(A)
        { }

        Real operator()(Real phi) const override {
            RealType value=0;

            for(auto n=1; n<=N; n+=2) {
                fix x = phi/A;
                value += get_lambda(n)/(n+1) * even_power(x, n+1);
            }

            return value;
        }

        static Vector<RealType>
        CalcCoefficients(int N) {
            Vector<RealType> coeffs;

            if(!(N%2)) throw Exception(Str(__PRETTY_FUNCTION__) + " works with odd-valued N.");
            if  (N<0)  throw Exception(Str(__PRETTY_FUNCTION__) + " works with positive-valued N.");

            for(auto n=1; n<=N; n+=2) {
                fix i=(n-1)/2;

                fix sign = (i%2)*2. - 1.;
                coeffs[i] = sign * lambda(n, N);
            }

            return coeffs;
        }
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_NONLINEARKGPOTENTIAL_H
