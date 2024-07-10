//
// Created by joao on 7/9/24.
//

#include "Viewer.h"

namespace Studios::Fields::Viewers {

    Viewer::Viewer(Slab::Pointer<Slab::Graphics::GUIWindow> gui_window, Slab::Pointer<Slab::Math::R2toR::NumericFunction> func)
    : Slab::Graphics::WindowPanel()
    , gui_window(std::move(gui_window))
    , base_function(func)
    {}

    void Viewer::setFunction(Slab::Pointer<Slab::Math::R2toR::NumericFunction> function) {
        base_function = std::move(function);
    }

    auto Viewer::getFunction() -> Slab::Pointer<Slab::Math::R2toR::NumericFunction> {
        return base_function;
    }

    void Viewer::beginGUI() { gui_window->begin(); }
    void Viewer::endGUI() { gui_window->end(); }

    void Viewer::notifyBecameVisible() {

    }

    void Viewer::notifyBecameInvisible() {

    }


} // Studios::Fields::Viewers