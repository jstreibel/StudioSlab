//
// Created by joao on 22/05/24.
//

#ifndef STUDIOSLAB_R2SECTIONARTIST_H
#define STUDIOSLAB_R2SECTIONARTIST_H

#include "Artist.h"

#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Function/RtoR2/ParametricCurve.h"

#include "Graphics/Graph/PlotStyle.h"

namespace Slab::Graphics {

    using namespace Math;

    class R2SectionArtist : public Artist {
        R2toR::Function_ptr function2D;
        Vector<Pair<RtoR2::ParametricCurve_ptr, PlotStyle>> sections;
        Resolution samples=1024;

    public:

        bool draw(const PlottingWindow &window) override;

        void setSamples(Resolution);
        void setFunction(R2toR::Function_ptr);
        auto getFunction() const -> R2toR::Function_constptr;

        void addSection(const RtoR2::ParametricCurve_ptr&, PlotStyle);

    };

    DefinePointer(R2SectionArtist)

} // Graphics

#endif //STUDIOSLAB_R2SECTIONARTIST_H
