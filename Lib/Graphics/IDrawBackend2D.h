//
// Created by joao on 12/3/25.
//

#ifndef STUDIOSLAB_IDRAWBACKEND2D_H
#define STUDIOSLAB_IDRAWBACKEND2D_H
#include "Math/VectorSpace/Impl/PointSet.h"
#include "Plot2D/PlotStyle.h"
#include "Utils/Result.h"

namespace Slab::Graphics {
struct IDrawBackend2D {
    virtual ~IDrawBackend2D() = default;

    virtual FResult DrawPointSet(const Math::FPointSet_constptr& pSet, const PlotStyle& style) const noexcept = 0;
    virtual FResult DrawPointSet(const Math::FPointSet& pSet, const PlotStyle& style) const noexcept = 0;

};
}

#endif //STUDIOSLAB_IDRAWBACKEND2D_H