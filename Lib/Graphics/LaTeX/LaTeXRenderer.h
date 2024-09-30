//
// Created by joao on 9/23/24.
//

#ifndef STUDIOSLAB_LATEXRENDERER_H
#define STUDIOSLAB_LATEXRENDERER_H

#include "3rdParty/NanoTeX/src/latex.h"

#include "LaTeX.h"

#include "Utils/String.h"
#include "Utils/Pointer.h"


namespace Slab::Graphics::LaTeX {

    class LaTeXRenderer {
        Pointer<tex::Graphics2D> graphics2D;
    public:
        explicit LaTeXRenderer();

        ~LaTeXRenderer();

        auto renderMath(const WStr& tex_math) -> Cairo::RefPtr<const Cairo::ImageSurface>;
    };

} // Slab::Graphics::LaTeX

#endif //STUDIOSLAB_LATEXRENDERER_H
