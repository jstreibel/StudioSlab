//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H
#define V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H

#include "Math/Graph/OpenGLMonitor.h"
#include "Core/Graphics/ZoomPanRotate.h"
#include "Math/Graph/FunctionGraph.h"
#include "Mappings/R2toR/View/Artists/GraphR2ToR.h"
#include "Mappings/RtoR2/StraightLine.h"


namespace R2toR {

    class OutputOpenGLShockwave : public Core::Graphics::OpenGLMonitor {
    public:
        OutputOpenGLShockwave(const NumericConfig &params);

        void draw() override;

        bool notifyMouseButton(int button, int dir, int x, int y) override;

        bool notifyMouseMotion(int x, int y) override;

        bool notifyKeyboardSpecial(int key, int x, int y) override;

        bool notifyKeyboard(unsigned char key, int x, int y) override;

        IntPair getWindowSizeHint() override;

        void addSection(const RtoR2::StraightLine &section, Str name);

    private:
        ZoomPanRotate zpr;
        bool showAnalytic = true;
        bool showDPhidt = false;

        GraphR2toR phiGraph, dPhiGraph;

        //std::vector<Base::SectionPair> sections;

        void _outputSnapshot();
    };

}


#endif //V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H
