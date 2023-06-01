//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H
#define V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H

#include "Base/Graphics/ARCHIVE/NotHere/OutputOpenGL.h"
#include "Base/Graphics/ZoomPanRotate.h"
#include "Phys/Graph/FunctionGraph.h"
#include "Fields/Mappings/R2toR/View/Artists/GraphR2ToR.h"

#include <Base/Graphics/ARCHIVE/deprecated/GraphCollections.h>

namespace R2toR {
    class OutputOpenGLShockwave : public Base::OutputOpenGL {
    public:
        OutputOpenGLShockwave();

        void draw() override;
        void notifyScreenReshape(int width, int height) override;

        void notifyMouseButton(int button, int dir, int x, int y) override;

        void notifyMouseMotion(int x, int y) override;

        void notifyKeyboardSpecial(int key, int x, int y) override;

        void notifyKeyboard(unsigned char key, int x, int y) override;

        IntPair getWindowSizeHint() override;

        void addSection(const RtoR2::StraightLine &section, String name);

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
