//
// Created by joao on 7/09/23.
//

#ifndef STUDIOSLAB_TRIGONOMETRIC_H
#define STUDIOSLAB_TRIGONOMETRIC_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class Sine : public RtoR::Function {
        DevFloat A, k;
    public:
        Sine(DevFloat a, DevFloat k);

        DevFloat operator()(DevFloat x) const override;

        Function_ptr diff(int n) const override;

        auto Clone() const -> Function_ptr override;
    };

    class Cosine : public RtoR::Function {
        DevFloat A, k;
    public:
        Cosine(DevFloat a, DevFloat k);

        DevFloat operator()(DevFloat x) const override;

        Function_ptr diff(int n) const override;

        auto Clone() const -> Function_ptr override;
    };

} // RtoR

#endif //STUDIOSLAB_TRIGONOMETRIC_H
