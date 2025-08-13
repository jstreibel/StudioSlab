//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_POINTSETARTIST_H
#define STUDIOSLAB_POINTSETARTIST_H

#include "Artist.h"
#include "Utils/Pointer.h"
#include "Math/VectorSpace/Impl/PointSet.h"
#include "Graphics/Plot2D/PlotStyle.h"

namespace Slab::Graphics {

    class PointSetArtist : public FArtist {
        Math::PointSet_ptr pointSet;
        PlotStyle plotStyle;

    public:
        PointSetArtist(Math::PointSet_ptr, PlotStyle);

        auto Draw(const FPlot2DWindow &) -> bool override;

        bool HasGUI() override;

        void DrawGUI() override;

        auto setPointSet(Math::PointSet_ptr) -> void;
        auto setStyle(PlotStyle style) -> void;

        auto GetRegion() -> const RectR & override;
    };

    DefinePointers(PointSetArtist)

} // Graphics

#endif //STUDIOSLAB_POINTSETARTIST_H
