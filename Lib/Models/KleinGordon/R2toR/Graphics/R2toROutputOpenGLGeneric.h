//
// Created by joao on 23/09/2019.
//

#ifndef R2TOR_OUTPUTMONITOR_GENERIC_H
#define R2TOR_OUTPUTMONITOR_GENERIC_H


#include "Graphics/BaseMonitor.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Artists/R2SectionArtist.h"

namespace Slab::Math::R2toR {



    class OutputOpenGL : public Graphics::BaseMonitor {
    protected:
        Graphics::Plot2DWindow mSectionGraph;
        Graphics::R2SectionArtist sectionArtist;

        Graphics::Plot2DWindow mFieldDisplay;

    public:
        OutputOpenGL(const NumericConfig &params);


        void draw() override;
    };

}


#endif //R2TOR_OUTPUTMONITOR_GENERIC_H
