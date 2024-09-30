//
// Created by joao on 9/26/24.
//

#ifndef STUDIOSLAB_LATEXTESTS_H
#define STUDIOSLAB_LATEXTESTS_H

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Graph/PlottingWindow.h"
#include "Graphics/LaTeX/LaTeXRenderer.h"

namespace Tests {

    class LaTeXTests : public Slab::Graphics::WindowRow {
        Slab::Graphics::GUIWindow stats;
        Slab::Graphics::PlottingWindow graph;

        Slab::Graphics::Writer writer;

        Slab::Graphics::LaTeX::LaTeXRenderer latexWriter;

    public:
        LaTeXTests();

        void draw() override;

        void notifyReshape(int w, int h) override;
    };

} // Tests

#endif //STUDIOSLAB_LATEXTESTS_H
