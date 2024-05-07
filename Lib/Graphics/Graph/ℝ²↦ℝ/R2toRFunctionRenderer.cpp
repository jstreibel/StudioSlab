//
// Created by joao on 06/05/24.
//

#include "R2toRFunctionRenderer.h"
#include "Maps/R2toR/Model/R2toRDiscreteFunctionCPU.h"

namespace Graphics {
    auto R2toRFunctionRenderer::renderToDiscrete(const R2toR::Function &function,
                                                 Resolution N, Resolution M,
                                                 R2toR::Domain domain, Real scale) -> std::shared_ptr<R2toR::DiscreteFunction> {
        fix xMin = domain.xMin;
        fix yMin = domain.yMin;
        fix dx = domain.getLx() / N;
        fix dy = domain.getLy() / M;
        auto discreteFunc = std::shared_ptr<R2toR::DiscreteFunction>(new R2toR::DiscreteFunction_CPU(N, M, xMin, yMin, dx, dy));

        return renderToDiscrete(function, discreteFunc, scale);
    }

    auto R2toRFunctionRenderer::renderToDiscrete(const R2toR::Function &function,
                                                 std::shared_ptr<R2toR::DiscreteFunction> out,
                                                 Real scale) -> std::shared_ptr<R2toR::DiscreteFunction> {
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

        return out;
    }
} // Graphics