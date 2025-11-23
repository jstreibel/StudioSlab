//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_POINTSETRENDERER_H
#define STUDIOSLAB_POINTSETRENDERER_H

#include "Math/VectorSpace/Impl/PointSet.h"
#include "Graphics/Plot2D/PlotStyle.h"

namespace Slab::Graphics::OpenGL::Legacy {
    bool RenderPointSet(const Math::FPointSet_constptr& pSet, const PlotStyle& style) noexcept;
    inline bool RenderPointSet(const Math::FPointSet& pSet, const PlotStyle& style) noexcept {
        // TODO: superbad below
        return RenderPointSet(Dummy(pSet), style);
    }

}

#endif //STUDIOSLAB_POINTSETRENDERER_H
