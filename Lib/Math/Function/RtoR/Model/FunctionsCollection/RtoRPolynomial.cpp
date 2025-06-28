//
// Created by joao on 10/31/24.
//

#include "RtoRPolynomial.h"

namespace Slab::Math::RtoR {

    RtoRPolynomial::RtoRPolynomial(Vector<DevFloat> coeffs) : coeffs(coeffs) {

    }

    DevFloat RtoRPolynomial::operator()(DevFloat x) const {
        double result = 0.0;

        // Iterate from the highest degree term to the lowest
        for (auto it = coeffs.rbegin(); it != coeffs.rend(); ++it)
            result = result * x + *it;

        return result;
    }

} // Slab::Math::RtoR