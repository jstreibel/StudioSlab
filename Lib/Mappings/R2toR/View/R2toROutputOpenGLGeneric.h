//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TOROUTPUTOPENGLGENERIC_H
#define V_SHAPE_R2TOROUTPUTOPENGLGENERIC_H

#include "Base/Graphics/ZoomPanRotate.h"
#include "Phys/Graph/OpenGLMonitor.h"
#include "Mappings/R2toR/View/Artists/GraphR2ToR.h"
#include "Base/Graphics/Window/WindowContainer/WindowPanel.h"
#include "Base/Graphics/Window/StatsDisplay.h"
#include "Mappings/RtoR/View/Graphics/GraphRtoR.h"
#include "Mappings/R2toR/View/Artists/GraphR2Section.h"
#include "Models/KleinGordon/R2toR/EquationState.h"

namespace R2toR {
    class OutputOpenGL : public Graphics::OpenGLMonitor {
    protected:
        Real phiMin, phiMax;
        GraphR2Section mSectionGraph;

    public:
        OutputOpenGL(const NumericParams &params, Real phiMin, Real phiMax);
        ~OutputOpenGL();


        void draw() override;
    };

}


#endif //V_SHAPE_R2TOROUTPUTOPENGLGENERIC_H
