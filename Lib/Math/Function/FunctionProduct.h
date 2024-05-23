//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_FUNCTIONPRODUCT_H
#define STUDIOSLAB_FUNCTIONPRODUCT_H

#include "Function.h"

namespace Slab::Math::Base {

        template<class InputCategory, class OutputCategory>
        class Product : public FunctionT<InputCategory, OutputCategory> {
            typedef FunctionT<InputCategory, OutputCategory> MyBase;

            const MyBase &A, &B;

        public:
            Product(MyBase &a, MyBase &b)
                : FunctionT<InputCategory, OutputCategory>(nullptr, false), A(a), B(b) { }

            Product(const Product &toCopy) : Product(toCopy.A, toCopy.B) { }


            friend MyBase::Pointer operator*(const MyBase &a, const MyBase &b) {
                return std::make_unique(a, b);
            }

            OutputCategory operator()(InputCategory x) const override { return A(x) * B(x); }
        };
}

#endif //STUDIOSLAB_FUNCTIONPRODUCT_H
