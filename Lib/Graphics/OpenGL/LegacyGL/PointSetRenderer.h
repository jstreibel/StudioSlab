//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_POINTSETRENDERER_H
#define STUDIOSLAB_POINTSETRENDERER_H

#include "Math/VectorSpace/Impl/PointSet.h"
#include "Graphics/Plot2D/PlotStyle.h"

namespace Slab::Graphics::OpenGL::Legacy {
    bool RenderPointSet(const Math::PointSet_constptr& pSet, PlotStyle style) noexcept;
}

#endif //STUDIOSLAB_POINTSETRENDERER_H
