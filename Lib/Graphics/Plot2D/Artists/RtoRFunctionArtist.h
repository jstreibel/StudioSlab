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

    class RtoRFunctionArtist : public FArtist {
        RtoR::Function_ptr function;
        PlotStyle plotStyle;
        CountType samples;

    public:
        RtoRFunctionArtist(RtoR::Function_ptr, PlotStyle, CountType samples);

        void setFunction(RtoR::Function_ptr);

        void setStyle(PlotStyle);

        void setSampling(CountType);

        const RectR &GetRegion() override;

        bool Draw(const FPlot2DWindow &d) override;

        bool HasGUI() override;

        void DrawGUI() override;
    };

    DefinePointers(RtoRFunctionArtist)

} // Graphics

#endif //STUDIOSLAB_RTORFUNCTIONARTIST_H
