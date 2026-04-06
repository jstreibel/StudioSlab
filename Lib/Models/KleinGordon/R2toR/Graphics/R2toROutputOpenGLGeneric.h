//
// Created by joao on 23/09/2019.
//

#ifndef R2TOR_OUTPUTMONITOR_GENERIC_H
#define R2TOR_OUTPUTMONITOR_GENERIC_H


#include "Graphics/BaseMonitor.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Artists/R2SectionArtist.h"

namespace Slab::Models::KGR2toR {

    class FOutputOpenGL : public Graphics::FBaseMonitor {

    protected:

        Graphics::FPlot2DWindow mSectionGraph;

        Graphics::R2SectionArtist sectionArtist;

        Graphics::FPlot2DWindow mFieldDisplay;

    public:

        explicit FOutputOpenGL(CountType max_steps);

        void ImmediateDraw(const Graphics::FPlatformWindow&) override;

    };

    using OutputOpenGL [[deprecated("Use FOutputOpenGL")]] = FOutputOpenGL;

}


#endif //R2TOR_OUTPUTMONITOR_GENERIC_H
