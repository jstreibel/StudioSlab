//
// Created by joao on 7/09/23.
//

#include "Trigonometric.h"

namespace Slab::Math::RtoR {
    FSine::FSine(DevFloat a, DevFloat k) : FunctionT(nullptr, false), A(a), k(k) {}

    DevFloat FSine::operator()(DevFloat x) const {
        return A*sin(k*x);
    }

    Function_ptr FSine::diff(int n) const {
        assert(n==0);

        return New<FCosine>(A*k, k);
    }

    Function_ptr FSine::Clone() const {
        return New<FSine>(A, k);
    }



    FCosine::FCosine(DevFloat a, DevFloat k) : FunctionT(nullptr, false), A(a), k(k) {}

    DevFloat FCosine::operator()(DevFloat x) const {
        return A*cos(k*x);
    }

    Function_ptr FCosine::diff(int n) const {
        assert(n==0);

        return New<FSine>(-A*k, k);
    }

    Function_ptr FCosine::Clone() const {
        return New<FCosine>(A, k);
    }
} // RtoR
