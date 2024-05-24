//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_POINTSETRENDERER_H
#define STUDIOSLAB_POINTSETRENDERER_H

#include "Math/Space/Impl/PointSet.h"
#include "Graphics/Graph/PlotStyle.h"

namespace Slab::Graphics::OpenGL::Legacy {
    bool RenderPointSet(Math::PointSet_constptr pSet, PlotStyle style) noexcept;
}

#endif //STUDIOSLAB_POINTSETRENDERER_H
