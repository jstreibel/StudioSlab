//
// Created by joao on 06/09/2019.
//

#ifndef V_SHAPE_ARITHMETICOPSINTERFACE_H
#define V_SHAPE_ARITHMETICOPSINTERFACE_H

#include "Utils/Types.h"
#include "Algebra.h"

#include "Operator.h"

/** Arithmetical Operations Interface */

namespace Slab::Math {

    template<class Ty>
    class NumericAlgebra;

#define MakeProxy(Operation)                                                        \
    class Operation##Proxy {                                                        \
        friend NumericAlgebra<Ty>;                                                  \
        const NumericAlgebra<Ty> &a, &b;                                            \
        Operation##Proxy(const NumericAlgebra<Ty> &a, const NumericAlgebra<Ty> &b)  \
        : a(a), b(b) {}                                                             \
        const Ty& get_a() const { return static_cast<const Ty&>(a); }               \
        const Ty& get_b() const { return static_cast<const Ty&>(b); }               \
    };

#define DefineOperation(Operation, Operator)                                        \
    private:                                                                        \
        MakeProxy(Operation)                                                        \
    public:                                                                         \
        virtual Ty &Store##Operation         (const Ty &, const Ty &) = 0;          \
        Operation##Proxy operator Operator (const NumericAlgebra<Ty>& other) const {\
            return Operation##Proxy(*this, other);                                  \
        }                                                                           \
        Ty& operator=(const Operation##Proxy &proxy) {                              \
            return this->Store##Operation(proxy.get_a(), proxy.get_b());            \
        }

    template<class Ty>
    class NumericAlgebra {
        using Oppy = Operator<Ty>;

        class ScalarMultiplicationProxy {
            friend NumericAlgebra<Ty>;
            const NumericAlgebra<Ty> &a;
            const Real b;
            ScalarMultiplicationProxy(const NumericAlgebra<Ty> &a, Real b)
            : a(a), b(b) {}

            const Ty& get_a() const { return static_cast<const Ty&>(a); }
            const Real& get_b() const { return b; }
        };

        DefineOperation(Addition, +)
        DefineOperation(Subtraction, -)

    public:
        /*
        // class Operation1 { virtual void operator()(Ty &state, Real a) = 0; };
         */
        struct MetaAlgebra {
            template<class Operation>
            static void for_each1(NumericAlgebra<Ty> &element, Operation operation) {
                operation(element);
            }
        };


        virtual Ty &Add                   (const Ty &) = 0;
        virtual Ty &Subtract              (const Ty &) = 0;

        virtual Ty &StoreScalarMultiplication   (const Ty &, Real a) = 0;
        virtual Ty &Multiply              (Real a) = 0;

        /*** *** ***
         * operators += and *= are sufficient for
         */
        Ty &operator+=(const Ty &a)     { return this->Add(a); }
        Ty &operator+=(const AdditionProxy &proxy)     {
            // TODO: must be a way to make this quicker
            this->Add(proxy.get_a());
            return this->Add(proxy.get_b());
        }
        Ty &operator*=(Real a)  { return this->Multiply(a); }

        Ty &operator-=(const Ty &a) { return this->Subtract(a); }

        virtual ScalarMultiplicationProxy operator * (Real a) const { return ScalarMultiplicationProxy(*this, a); }
        Ty& operator=(const ScalarMultiplicationProxy &proxy) {
            return this->StoreScalarMultiplication(proxy.get_a(), proxy.get_b());
        }

        Ty& operator=(const Operator<Ty>::OperatorActionProxy& proxyDoxy) {
            auto &oppy = proxyDoxy.O;
            auto &g = proxyDoxy.g;

            auto &typey = static_cast<Ty&>(*this);

            return oppy.operateAndStoreResult(typey, g);
        }
    };
}

#endif //V_SHAPE_ARITHMETICOPSINTERFACE_H