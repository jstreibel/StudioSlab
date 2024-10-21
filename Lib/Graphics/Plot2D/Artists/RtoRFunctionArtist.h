//
// Created by joao on 21/05/24.
//

#ifndef STUDIOSLAB_RTORFUNCTIONARTIST_H
#define STUDIOSLAB_RTORFUNCTIONARTIST_H

#include "Artist.h"
#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Graphics/Plot2D/PlotStyle.h"

namespace Slab::Graphics {

    using namespace Math;

    class RtoRFunctionArtist : public Artist {
        RtoR::Function_ptr function;
        PlotStyle plotStyle;
        Count samples;

    public:
        RtoRFunctionArtist(RtoR::Function_ptr, PlotStyle, Count samples);

        void setFunction(RtoR::Function_ptr);

        void setStyle(PlotStyle);

        void setSampling(Count);

        const RectR &getRegion() override;

        bool draw(const Plot2DWindow &d) override;

        bool hasGUI() override;

        void drawGUI() override;
    };

    DefinePointers(RtoRFunctionArtist)

} // Graphics

#endif //STUDIOSLAB_RTORFUNCTIONARTIST_H
