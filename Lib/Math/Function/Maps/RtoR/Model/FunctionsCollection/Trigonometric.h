//
// Created by joao on 7/09/23.
//

#ifndef STUDIOSLAB_TRIGONOMETRIC_H
#define STUDIOSLAB_TRIGONOMETRIC_H

#include "Math/Function/Maps/RtoR/Model/RtoRFunction.h"

namespace RtoR {

    class Sine : public RtoR::Function {
        Real A, k;
    public:
        Sine(Real a, Real k);

        Real operator()(Real x) const override;

        Ptr diff(int n) const override;
    };

    class Cosine : public RtoR::Function {
        Real A, k;
    public:
        Cosine(Real a, Real k);

        Real operator()(Real x) const override;

        Ptr diff(int n) const override;
    };

} // RtoR

#endif //STUDIOSLAB_TRIGONOMETRIC_H
