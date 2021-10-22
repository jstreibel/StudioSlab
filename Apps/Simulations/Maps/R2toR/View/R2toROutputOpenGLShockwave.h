//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H
#define V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H

#include <View/Graphic/Tools/ZoomPanRotate.h>
#include <Apps/Simulations/Maps/R2toR/View/R2toROutputOpenGLGeneric.h>
#include <View/Graphic/Tools/GraphCollections.h>
#include "Model/MathTypes.h"
#include "View/Graphic/OutputOpenGL.h"
#include "Apps/Simulations/Maps/RtoR2/StraightLine.h"


namespace R2toR {
    class OutputOpenGLShockwave : public Base::OutputOpenGL {
    public:
        OutputOpenGLShockwave(UserParamMap userParamMap);

        void draw() override;
        void reshape(int width, int height) override;

        void notifyGLUTMouseButton(int button, int dir, int x, int y) override;

        void notifyGLUTMouseMotion(int x, int y) override;

        void notifyGLUTKeyboardSpecial(int key, int x, int y) override;

        void notifyGLUTKeyboard(unsigned char key, int x, int y) override;

        IntPair getWindowSizeHint() override;

        void addSection(const RtoR2::StraightLine &section, String name);

    private:
        ZoomPanRotate zpr;
        bool showAnalytic = true;
        bool showDPhidt = false;

        Real xMin, xMax, yMin, yMax, phiMin, phiMax;
        UserParamMap userParamMap;
        Graph phiGraph, dPhiGraph;

        std::vector<Base::SectionPair> sections;

        void _outputSnapshot();
    };

}


#endif //V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H
