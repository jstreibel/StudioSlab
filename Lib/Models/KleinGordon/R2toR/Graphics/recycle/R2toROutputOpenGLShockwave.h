//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H
#define V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H

#include "Graphics/BaseMonitor.h"
#include "Graphics/ZoomPanRotate.h"
#include "Graphics/Plot2D/FunctionGraph_depr.h"
#include "Math/Function/R2toR/View/GraphR2ToR.h"
#include "Math/Function/RtoR2/StraightLine.h"


namespace Slab::Math::R2toR {

    class OutputOpenGLShockwave : public Graphics::OpenGLMonitor {
    public:
        OutputOpenGLShockwave(const NumericConfig &params);

        void draw() override;

        bool notifyMouseMotion(int x, int y, int dx, int dy) override;

        bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        IntPair getWindowSizeHint() override;

        void addSection(const RtoR2::StraightLine &section, Str name);

    private:
        ZoomPanRotate zpr;
        bool showAnalytic = true;
        bool showDPhidt = false;

        GraphR2toR phiGraph, dPhiGraph;

        //Vector<Base::SectionPair> sections;

        void _outputSnapshot();
    };

}


#endif //V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H
