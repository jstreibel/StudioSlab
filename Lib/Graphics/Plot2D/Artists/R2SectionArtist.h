//
// Created by joao on 22/05/24.
//

#ifndef STUDIOSLAB_R2SECTIONARTIST_H
#define STUDIOSLAB_R2SECTIONARTIST_H

#include "Artist.h"

#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Function/RtoR2/ParametricCurve.h"

#include "Graphics/Plot2D/PlotStyle.h"

namespace Slab::Graphics {

    using namespace Math;

    class R2SectionArtist : public Artist {
        R2toR::Function_constptr function2D;

        struct SectionData {
            RtoR2::ParametricCurve_ptr section;
            Pointer<PlotStyle> style;
            Str name;
            bool visible = true;
        };

        Vector<SectionData> sections;
        Resolution samples=1024;

    public:

        bool draw(const Plot2DWindow &window) override;

        void setSamples(Resolution);
        void setFunction(R2toR::Function_constptr);
        auto getFunction() const -> R2toR::Function_constptr;

        void addSection(const RtoR2::ParametricCurve_ptr&, const Pointer<PlotStyle>&, const Str& name);
        void clearSections();

        const RectR &getRegion() override;

        bool hasGUI() override;

        void drawGUI() override;

    };

    DefinePointers(R2SectionArtist)

} // Graphics

#endif //STUDIOSLAB_R2SECTIONARTIST_H
