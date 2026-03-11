//
// Created by joao on 9/26/24.
//

#include "LaTeX.h"

#define BUILD_GTK
#include "3rdParty/NanoTeX/src/platform/cairo/graphic_cairo.cpp"
#undef BUILD_GTK

#include "Core/Tools/Resources.h"

#include <mutex>

namespace Slab::Graphics::LaTeX {

    void Init() {
        static std::once_flag initOnce;
        std::call_once(initOnce, []() {
            tex::LaTeX::init(Core::Resources::NanoTeXFolder);
        });
    }

    void Release() {
        // NanoTeX's bundled release path is not stable in this environment.
        // Keep the shared runtime alive for the process lifetime.
    }

}
