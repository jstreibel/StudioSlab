//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_POINTSETARTIST_H
#define STUDIOSLAB_POINTSETARTIST_H

#include "Artist.h"
#include "Utils/Pointer.h"
#include "Math/Space/Impl/PointSet.h"
#include "Graphics/Graph/PlotStyle.h"

namespace Slab::Graphics {

    class PointSetArtist : public Artist {
        Math::PointSet_ptr pointSet;
        PlotStyle plotStyle;

    public:
        PointSetArtist(Math::PointSet_ptr, PlotStyle);

        bool draw(const PlottingWindow &) override;

        const RectR &getRegion() override;
    };

    DefinePointer(PointSetArtist)

} // Graphics

#endif //STUDIOSLAB_POINTSETARTIST_H
