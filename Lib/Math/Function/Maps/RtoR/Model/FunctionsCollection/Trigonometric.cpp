//
// Created by joao on 7/09/23.
//

#include "Trigonometric.h"

namespace RtoR {
    Sine::Sine(Real a, Real k) : FunctionT(nullptr, false), A(a), k(k) {}

    Real Sine::operator()(Real x) const {
        return A*sin(k*x);
    }

    Core::FunctionT<Real, Real>::Ptr Sine::diff(int n) const {
        assert(n==0);

        return Ptr(new Cosine(A*k, k));
    }

    Cosine::Cosine(Real a, Real k) : FunctionT(nullptr, false), A(a), k(k) {}

    Real Cosine::operator()(Real x) const {
        return A*cos(k*x);
    }

    Core::FunctionT<Real, Real>::Ptr Cosine::diff(int n) const {
        assert(n==0);

        return Ptr(new Sine(-A*k, k));
    }
} // RtoR