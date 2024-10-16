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
        OutputOpenGL(Real t_max, Count max_steps, Real x_min, Real x_max, Real y_min, Real y_max);


        void draw() override;
    };

}


#endif //R2TOR_OUTPUTMONITOR_GENERIC_H
