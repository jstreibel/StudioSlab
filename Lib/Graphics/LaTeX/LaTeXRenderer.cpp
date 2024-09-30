//
// Created by joao on 9/23/24.
//

#include "LaTeXRenderer.h"

#include "Core/Tools/Log.h"

namespace Slab::Graphics::LaTeX {

    Cairo::RefPtr<Cairo::ImageSurface> cairo_surface;
    Cairo::RefPtr<Cairo::Context> cairo_context;

    LaTeXRenderer::LaTeXRenderer() {
        LaTeX::Init();

        cairo_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 200, 200);
        cairo_context = Cairo::Context::create(cairo_surface);

        graphics2D = Slab::New<tex::Graphics2D_cairo>(cairo_context);
    }

    LaTeXRenderer::~LaTeXRenderer() {
        LaTeX::Release();
    }

    Cairo::RefPtr<const Cairo::ImageSurface> LaTeXRenderer::renderMath(const WStr& tex_math) {
        auto render = tex::LaTeX::parse(
                tex_math,
                600,
                40,
                40 / 3.f,
                0xff424242);

        render->draw(*graphics2D, 100, 100);
    }

} // Slab::Graphics::LaTeX