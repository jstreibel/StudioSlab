//
// Created by joao on 8/09/23.
//

#ifndef STUDIOSLAB_BEZIERTESTS_H
#define STUDIOSLAB_BEZIERTESTS_H

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Plot2D/Plot2DWindow.h"


namespace Tests {

    using namespace Slab;

    class BezierTests : public Graphics::WindowRow {
        Graphics::GUIWindow stats;
        Graphics::Plot2DWindow graph;

        Math::PointSet currentPt;

    public:
        BezierTests();

        void draw() override;
    };

} // Tests

#endif //STUDIOSLAB_BEZIERTESTS_H
