//
// Created by joao on 21/05/24.
//

#ifndef STUDIOSLAB_RTORFUNCTIONARTIST_H
#define STUDIOSLAB_RTORFUNCTIONARTIST_H

#include "Artist.h"
#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Graphics/Graph/PlotStyle.h"

namespace Graphics {

    class RtoRFunctionArtist : public Artist {
        RtoR::Function_ptr function;
        PlotStyle plotStyle;
        Count samples;


    public:
        RtoRFunctionArtist(RtoR::Function_ptr, PlotStyle, Count samples);

        void draw(const PlottingWindow &d) override;
    };

    DefinePointer(RtoRFunctionArtist)

} // Graphics

#endif //STUDIOSLAB_RTORFUNCTIONARTIST_H
