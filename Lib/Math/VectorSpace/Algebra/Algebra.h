//
// Created by joao on 5/29/24.
//

#ifndef STUDIOSLAB_ALGEBRA_H
#define STUDIOSLAB_ALGEBRA_H

#include "AlgebraProxies.h"

namespace Slab::Math {



    class Algebra {
    public:
        virtual bool isProxy() const { return false; }

        virtual Algebra& operator+(const Algebra&) = 0;
        virtual Algebra& operator*(const Algebra&) = 0;
        virtual Algebra& operator*(const Real&a) { return (*this)*=a; };
        virtual Algebra& operator*(const Real&a) const = 0;
        virtual Algebra& operator*=(const Real&) = 0;
    };

    class AlgebraProxy : public Algebra {
        friend class Algebra;
        const Algebra &a;
        const Algebra &b;

        enum ProxyType {
            Summation,
            Subtraction
        };

    public:
        bool isProxy() const override { return true; }

        AlgebraProxy(const Algebra&a, const Algebra&b) : a(a), b(b) {};

        Algebra &operator+(const Algebra &algebra) override {
            NOT_IMPLEMENTED_CLASS_METHOD
        }

        Algebra &operator*(const Algebra &algebra) override {
            NOT_IMPLEMENTED_CLASS_METHOD
        }

        Algebra &operator*(const Real &a) const override {
            NOT_IMPLEMENTED_CLASS_METHOD
        }

        Algebra &operator*=(const Real &real) override {
            NOT_IMPLEMENTED_CLASS_METHOD
        }

    };

    class AlgebraScalarProxy : public Algebra {

    };

}

#endif //STUDIOSLAB_ALGEBRA_H
