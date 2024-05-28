//
// Created by joao on 7/09/23.
//

#include "Trigonometric.h"

namespace Slab::Math::RtoR {
    Sine::Sine(Real a, Real k) : FunctionT(nullptr, false), A(a), k(k) {}

    Real Sine::operator()(Real x) const {
        return A*sin(k*x);
    }

    Function_ptr Sine::diff(int n) const {
        assert(n==0);

        return New<Cosine>(A*k, k);
    }

    Function_ptr Sine::Clone() const {
        return New<Sine>(A, k);
    }



    Cosine::Cosine(Real a, Real k) : FunctionT(nullptr, false), A(a), k(k) {}

    Real Cosine::operator()(Real x) const {
        return A*cos(k*x);
    }

    Function_ptr Cosine::diff(int n) const {
        assert(n==0);

        return New <Sine> (-A*k, k);
    }

    Function_ptr Cosine::Clone() const {
        return New <Cosine> (A, k);
    }
} // RtoR