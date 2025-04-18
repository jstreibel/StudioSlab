//
// Created by joao on 10/31/24.
//

#ifndef STUDIOSLAB_RTORPOLYNOMIAL_H
#define STUDIOSLAB_RTORPOLYNOMIAL_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class RtoRPolynomial : public RtoR::Function {
        Vector<Real> coeffs;
    public:
        RtoRPolynomial(Vector<Real> coeffs);

        Real operator()(Real x) const override;
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_RTORPOLYNOMIAL_H
