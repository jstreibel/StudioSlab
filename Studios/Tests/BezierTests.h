//
// Created by joao on 8/09/23.
//

#ifndef STUDIOSLAB_BEZIERTESTS_H
#define STUDIOSLAB_BEZIERTESTS_H

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Graph/Graph.h"

#include "Maps/RtoR/View/GraphRtoR.h"

namespace Tests {

    class BezierTests : public WindowRow {
        GUIWindow stats;
        GraphRtoR graph;

        Spaces::PointSet currentPt;

    public:
        BezierTests();

        void draw() override;
    };

} // Tests

#endif //STUDIOSLAB_BEZIERTESTS_H
