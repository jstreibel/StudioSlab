//
// Created by joao on 5/09/23.
//

#include "InverseFourier.h"

namespace Slab::Math::RtoR {
    DevFloat InverseFourier::operator()(DevFloat x) const {
        auto N = modes.re->GetPoints().size();

        IN re = modes.re->GetPoints();
        IN im = modes.im->GetPoints();

        DevFloat val = 0.0;

        // First term must be 1/2
        {
            fix Re_A = re[0].y;
            fix Im_A = im[0].y;

            fix A = sqrt(Re_A * Re_A + Im_A * Im_A);
            fix ϕ = atan2(Im_A, Re_A);

            val += 0.5 * A * cos(ϕ);
        }

        for(int i=1; i<N; ++i) {
            fix k = re[i].x;
            fix Re_A = re[i].y;
            fix Im_A = im[i].y;

            fix A = sqrt(Re_A*Re_A + Im_A*Im_A);
            fix ϕ = atan2(Im_A, Re_A);

            val += A*cos(k*x + ϕ);
        }

        return val;
    }
} // RtoR