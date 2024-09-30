//
// Created by joao on 9/26/24.
//

#include "LaTeX.h"

#define BUILD_GTK
#include "3rdParty/NanoTeX/src/platform/cairo/graphic_cairo.cpp"
#undef BUILD_GTK

#include "Core/Tools/Resources.h"

namespace Slab::Graphics::LaTeX {

    void Init() {
        tex::LaTeX::init(Core::Resources::NanoTeXFolder);
    }

    void Release() {
        tex::LaTeX::release();
    }

}
