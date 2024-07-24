//
// Created by joao on 7/24/24.
//

#ifndef STUDIOSLAB_VSHAPEEXPANSIONTEST_H
#define STUDIOSLAB_VSHAPEEXPANSIONTEST_H

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Graph/PlottingWindow.h"

namespace Tests {

    class VShapeExpansionTest : public Slab::Graphics::WindowRow {
        Slab::Graphics::GUIWindow stats;
        Slab::Graphics::PlottingWindow graph;

    public:
        VShapeExpansionTest();

        void draw() override;
    };

} // Tests

#endif //STUDIOSLAB_VSHAPEEXPANSIONTEST_H
