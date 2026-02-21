//
// Created by joao on 7/09/23.
//

#ifndef STUDIOSLAB_TRIGONOMETRIC_H
#define STUDIOSLAB_TRIGONOMETRIC_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class FSine : public RtoR::Function {
        DevFloat A, k;
    public:
        FSine(DevFloat a, DevFloat k);

        DevFloat operator()(DevFloat x) const override;

        Function_ptr diff(int n) const override;

        auto Clone() const -> Function_ptr override;
    };

    class FCosine : public RtoR::Function {
        DevFloat A, k;
    public:
        FCosine(DevFloat a, DevFloat k);

        DevFloat operator()(DevFloat x) const override;

        Function_ptr diff(int n) const override;

        auto Clone() const -> Function_ptr override;
    };

    using Sine [[deprecated("Use FSine")]] = FSine;
    using Cosine [[deprecated("Use FCosine")]] = FCosine;

} // RtoR

#endif //STUDIOSLAB_TRIGONOMETRIC_H
