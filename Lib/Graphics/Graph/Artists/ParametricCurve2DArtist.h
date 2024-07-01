//
// Created by joao on 21/05/24.
//

#ifndef STUDIOSLAB_PARAMETRICCURVE2DARTIST_H
#define STUDIOSLAB_PARAMETRICCURVE2DARTIST_H

#include "Artist.h"
#include "Graphics/Graph/PlotStyle.h"
#include "Math/Function/RtoR2/ParametricCurve.h"

namespace Slab::Graphics {

    using namespace Math;

    class ParametricCurve2DArtist : public Artist {
        Slab::Pointer<RtoR2::ParametricCurve> curve;
        PlotStyle plotStyle;

    public:
        ParametricCurve2DArtist(Slab::Pointer<RtoR2::ParametricCurve>, PlotStyle);

        bool draw(const PlottingWindow &d) override;

        void setCurve(Slab::Pointer<RtoR2::ParametricCurve>);

    };

    DefinePointers(ParametricCurve2DArtist)

} // Graphics

#endif //STUDIOSLAB_PARAMETRICCURVE2DARTIST_H
