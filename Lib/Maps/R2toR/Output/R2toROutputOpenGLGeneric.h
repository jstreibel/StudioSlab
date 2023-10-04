//
// Created by joao on 23/09/2019.
//

#ifndef R2TOR_OUTPUTMONITOR_GENERIC_H
#define R2TOR_OUTPUTMONITOR_GENERIC_H


#include "Graphics/ZoomPanRotate.h"
#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/GUIWindow.h"

#include "Graphics/OpenGL/OpenGLMonitor.h"

#include "Maps/RtoR/View/GraphRtoR.h"

#include "Maps/R2toR/View/FlatFieldDisplay.h"
#include "Maps/R2toR/View/GraphR2ToR.h"
#include "Maps/R2toR/View/GraphR2Section.h"

#include "Models/KleinGordon/R2toR/EquationState.h"

namespace R2toR {
class OutputOpenGL : public ::Graphics::OpenGLMonitor {
    protected:
        Real phiMin, phiMax;
        GraphR2Section mSectionGraph;
        Graphics::FlatFieldDisplay mFieldDisplay;

    public:
        OutputOpenGL(const NumericConfig &params, Real phiMin, Real phiMax);


        void draw() override;
    };

}


#endif //R2TOR_OUTPUTMONITOR_GENERIC_H
