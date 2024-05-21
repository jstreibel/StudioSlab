//
// Created by joao on 21/05/24.
//

#ifndef STUDIOSLAB_PARAMETRICCURVE2DARTIST_H
#define STUDIOSLAB_PARAMETRICCURVE2DARTIST_H

#include "Artist.h"
#include "Graphics/Graph/PlotStyle.h"

namespace RtoR2 { class ParametricCurve; }

namespace Graphics {

    class ParametricCurve2DArtist : public Artist {
        Slab::Pointer<RtoR2::ParametricCurve> curve;
        PlotStyle plotStyle;

    public:
        ParametricCurve2DArtist(Slab::Pointer<RtoR2::ParametricCurve>, PlotStyle);

        void draw(const Graph2D &d) override;

        void setCurve(Slab::Pointer<RtoR2::ParametricCurve>);

    };

    DefinePointer(ParametricCurve2DArtist)

} // Graphics

#endif //STUDIOSLAB_PARAMETRICCURVE2DARTIST_H
