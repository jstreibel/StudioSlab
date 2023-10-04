//
// Created by joao on 8/09/23.
//

#ifndef STUDIOSLAB_BEZIERTESTS_H
#define STUDIOSLAB_BEZIERTESTS_H

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Graph/Graph.h"
#include "Graphics/Graph/ℝ↦ℝ/GraphRtoR.h"


namespace Tests {

    class BezierTests : public Graphics::WindowRow {
        Graphics::GUIWindow stats;
        Graphics::GraphRtoR graph;

        Spaces::PointSet currentPt;

    public:
        BezierTests();

        void draw() override;
    };

} // Tests

#endif //STUDIOSLAB_BEZIERTESTS_H
