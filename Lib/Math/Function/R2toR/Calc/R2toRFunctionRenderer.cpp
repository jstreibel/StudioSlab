//
// Created by joao on 06/05/24.
//

#include "R2toRFunctionRenderer.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

namespace Slab::Math::R2toR {
    auto R2toRFunctionRenderer::renderToDiscrete(const R2toR::Function &function,
                                                 Resolution N, Resolution M,
                                                 R2toR::Domain domain, DevFloat scale) -> R2toR::FNumericFunction_ptr {
        fix xMin = domain.xMin;
        fix yMin = domain.yMin;
        fix dx = domain.getLx() / N;
        fix dy = domain.getLy() / M;
        auto discreteFunc = DataAlloc<R2toR::NumericFunction_CPU>(function.Symbol() + " [rendered]", N, M, xMin, yMin, dx, dy);

        return renderToDiscrete(function, discreteFunc, scale);
    }

    auto R2toRFunctionRenderer::renderToDiscrete(const R2toR::Function &function,
                                                 R2toR::FNumericFunction_ptr out,
                                                 DevFloat scale) -> R2toR::FNumericFunction_ptr {
        assert(out != nullptr);

        fix &domain = out->getDomain();
        fix N = out->getN();
        fix M = out->getM();
        fix xMin = domain.xMin;
        fix yMin = domain.yMin;
        fix dx = domain.getLx() / N;
        fix dy = domain.getLy() / M;

        auto &data = out->getSpace().getHostData(false);

        for(int i=0; i<N; ++i) {
            fix x = xMin + dx*(i+.5);
            for (int j = 0; j < M; ++j) {
                fix y = yMin + dy*(j+.5);

                data[i+j*N] = scale*function({x,y});
            }
        }

        std::cout << std::flush;

        return out;
    }
} // Graphics