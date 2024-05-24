//
// Created by joao on 23/09/2019.
//

#ifndef R2TOR_OUTPUTMONITOR_GENERIC_H
#define R2TOR_OUTPUTMONITOR_GENERIC_H


#include "Graphics/OpenGL/OpenGLMonitor.h"
#include "Graphics/Graph/PlottingWindow.h"
#include "Graphics/Graph/Artists/R2SectionArtist.h"

namespace Slab::Math::R2toR {



    class OutputOpenGL : public Graphics::OpenGLMonitor {
    protected:
        Graphics::PlottingWindow mSectionGraph;
        Graphics::R2SectionArtist sectionArtist;

        Graphics::PlottingWindow mFieldDisplay;

    public:
        OutputOpenGL(const NumericConfig &params);


        void draw() override;
    };

}


#endif //R2TOR_OUTPUTMONITOR_GENERIC_H
