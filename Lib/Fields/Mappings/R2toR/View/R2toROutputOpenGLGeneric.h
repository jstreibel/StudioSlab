//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H
#define V_SHAPE_R2TOROUTPUTOPENGLGENERIC_H

#include "Studios/Math/Maps/RtoR2/StraightLine.h"

#include <Studios/Graphics/ZoomPanRotate.h>
#include <Phys/DiffMath/Util/MathTypes.h>
#include <Studios/Graphics/NotHere/OutputOpenGL.h>

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

    private:
        ZoomPanRotate zpr;
        bool showAnalytic = true;

        Real xMin, xMax, yMin, yMax, phiMin, phiMax;
        Graph phiGraph, dPhiGraph;

    };

}


#endif //V_SHAPE_R2TOROUTPUTOPENGLSHOCKWAVE_H
