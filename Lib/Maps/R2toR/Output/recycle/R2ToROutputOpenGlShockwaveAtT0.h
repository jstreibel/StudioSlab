//
// Created by joao on 28/05/2021.
//

#ifndef V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVEATT0_H
#define V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVEATT0_H


#include "R2toROutputOpenGLShockwave.h"
#include "Core/Graphics/ZoomPanRotate.h"
#include "Math/Graph/FunctionGraph.h"
#include "Maps/RtoR2/StraightLine.h"


namespace R2toR {

    class OutputOpenGLShockwaveAt_t0 : public Core::Graphics::OpenGLMonitor {
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

        //std::vector<Base::SectionPair> sections;

        void _outputSnapshot();
    };

}


#endif //V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVEATT0_H