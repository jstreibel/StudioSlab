//
// Created by joao on 9/26/24.
//

#ifndef STUDIOSLAB_LATEXTESTS_H
#define STUDIOSLAB_LATEXTESTS_H

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/LaTeX/LaTeXWriter.h"
#include "Graphics/OpenGL/WriterOpenGL.h"

namespace Tests {

    using Writer = Slab::Graphics::OpenGL::FWriterOpenGL;

    class LaTeXTests : public Slab::Graphics::FWindowRow {
        Slab::Graphics::FGUIWindow stats;
        Slab::Graphics::FPlot2DWindow graph;

        Writer writer;

        Slab::Graphics::LaTeX::LaTeXWriter latexWriter;

    public:
        LaTeXTests();

        void ImmediateDraw() override;

        void NotifyReshape(int w, int h) override;
    };

} // Tests

#endif //STUDIOSLAB_LATEXTESTS_H
