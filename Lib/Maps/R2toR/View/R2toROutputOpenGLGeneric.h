//
// Created by joao on 23/09/2019.
//

#ifndef R2TOR_OUTPUTMONITOR_GENERIC_H
#define R2TOR_OUTPUTMONITOR_GENERIC_H


#include "Core/Graphics/ZoomPanRotate.h"
#include "Core/Graphics/Window/WindowContainer/WindowPanel.h"
#include "Core/Graphics/Window/GUIWindow.h"

#include "Math/Graph/OpenGLMonitor.h"

#include "Maps/R2toR/View/Artists/FlatFieldDisplay.h"
#include "Maps/RtoR/View/Graphics/GraphRtoR.h"
#include "Maps/R2toR/View/Artists/GraphR2ToR.h"
#include "Maps/R2toR/View/Artists/GraphR2Section.h"

#include "Models/KleinGordon/R2toR/EquationState.h"

namespace R2toR {
    class OutputOpenGL : public Core::Graphics::OpenGLMonitor {
    protected:
        Real phiMin, phiMax;
        GraphR2Section mSectionGraph;
        R2toR::Graphics::FlatFieldDisplay mFieldDisplay;

    public:
        OutputOpenGL(const NumericConfig &params, Real phiMin, Real phiMax);


        void draw() override;
    };

}


#endif //R2TOR_OUTPUTMONITOR_GENERIC_H
