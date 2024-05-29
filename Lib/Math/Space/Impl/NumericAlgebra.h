//
// Created by joao on 06/09/2019.
//

#ifndef V_SHAPE_ARITHMETICOPSINTERFACE_H
#define V_SHAPE_ARITHMETICOPSINTERFACE_H

#include "Utils/Types.h"
#include "Math/Space/Algebra.h"

/** Arithmetical Operations Interface */

namespace Slab::Math {

    template<class Ty>
    class NumericAlgebra;

    template<class Ty>
    class SummationProxy {
        friend NumericAlgebra<Ty>;
        const NumericAlgebra<Ty> &a, &b;

        SummationProxy(const NumericAlgebra<Ty> &a, const NumericAlgebra<Ty> &b) : a(a), b(b) {}

        const Ty& get_a() const {
            return static_cast<const Ty&>(a);
        }

        const Ty& get_b() const {
            return static_cast<const Ty&>(b);
        }
    };

    template<class Ty>
    class NumericAlgebra {
    public:

        virtual Ty &Add                   (const Ty &toi) = 0;
        virtual Ty &Subtract              (const Ty &toi) = 0;

        virtual Ty &StoreAddition         (const Ty &toi1, const Ty &toi2) = 0;
        virtual Ty &StoreSubtraction      (const Ty &aoi1, const Ty &aoi2) = 0;
        virtual Ty &StoreMultiplication   (const Ty &aoi1, Real a) = 0;
        virtual Ty &Multiply              (Real a) = 0;

        Ty &operator+=(const Ty &aoi) { return this->Add(aoi); }
        Ty &operator-=(const Ty &aoi) { return this->Subtract(aoi); }

        virtual Ty &operator*=(Real a)      { return this->Multiply(a); }

        SummationProxy<Ty> operator+(const NumericAlgebra<Ty>& other) const { return SummationProxy<Ty>(*this, other); }

        Ty& operator=(const SummationProxy<Ty> &proxy) {
            return this->StoreAddition(proxy.get_a(), proxy.get_b());
        }
    };
}

#endif //V_SHAPE_ARITHMETICOPSINTERFACE_H