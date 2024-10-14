//
// Created by joao on 28/05/2021.
//

#ifndef V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVEATT0_H
#define V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVEATT0_H


#include "R2toROutputOpenGLShockwave.h"
#include "Graphics/ZoomPanRotate.h"
#include "Graphics/Plot2D/FunctionGraph_depr.h"
#include "Math/Function/RtoR2/StraightLine.h"


namespace Slab::Math::R2toR {

    class OutputOpenGLShockwaveAt_t0 : public Graphics::OpenGLMonitor {
    public:
        OutputOpenGLShockwaveAt_t0(const NumericConfig &params);

        void draw() override;

        bool notifyMouseButton(int button, int dir, int x, int y) override;

        bool notifyMouseMotion(int x, int y) override;

        bool notifyKeyboardSpecial(int key, int x, int y) override;

        bool notifyKeyboard(unsigned char key, int x, int y) override;

        IntPair getWindowSizeHint() override;

    private:
        ZoomPanRotate zpr;
        bool showAnalytic = true;
        bool showDPhidt = false;

        Real xMin, xMax, yMin, yMax, phiMin, phiMax;
        GraphR2toR phiGraph, dPhiGraph;

        //Vector<Base::SectionPair> sections;

        void _outputSnapshot();
    };

}


#endif //V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVEATT0_H
