//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TOROUTPUTOPENGLGENERIC_H
#define V_SHAPE_R2TOROUTPUTOPENGLGENERIC_H

#include <Studios/Graphics/ZoomPanRotate.h>
#include "Studios/Graphics/ARCHIVE/NotHere/OutputOpenGL.h"
#include "Artists/GraphR2ToR.h"
#include "Studios/Graphics/WindowManagement/WindowContainer/WindowPanel.h"
#include "Studios/Graphics/Artists/StatsDisplay.h"
#include "Fields/Mappings/RtoR/View/Graphics/Artists/GraphRtoR.h"
#include "Fields/Mappings/R2toR/View/Artists/GraphR2Section.h"

namespace R2toR {
    class OutputOpenGL : public Base::OutputOpenGL {
    public:
        OutputOpenGL(Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax);


        void draw() override;
        void notifyReshape(int width, int height) override;

        void notifyMouseButton(int button, int dir, int x, int y) override;

        void notifyMouseMotion(int x, int y) override;

        void notifyKeyboardSpecial(int key, int x, int y) override;

        void notifyKeyboard(unsigned char key, int x, int y) override;

        IntPair getWindowSizeHint() override;

    protected:
        WindowPanel *panel;
        StatsDisplay stats;

        bool showAnalytic = true;

        Real xMin, xMax, yMin, yMax, phiMin, phiMax;
        GraphR2toR mPhiGraph;
        GraphR2Section mSectionGraph;

    };

}


#endif //V_SHAPE_R2TOROUTPUTOPENGLGENERIC_H
