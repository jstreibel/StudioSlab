//
// Created by joao on 7/9/24.
//

#include "Viewer.h"

namespace Slab::Graphics {

    Viewer::Viewer(Pointer<GUIWindow> gui_window, Pointer<Math::R2toR::NumericFunction> func)
    : Graphics::WindowPanel()
    , gui_window(std::move(gui_window))
    , base_function(func)
    {}

    void Viewer::setFunction(Pointer<Math::R2toR::NumericFunction> function) {
        base_function = std::move(function);
    }

    auto Viewer::getFunction() const -> Pointer<const Math::R2toR::NumericFunction> {
        return base_function;
    }

    void Viewer::beginGUI() { gui_window->begin(); }
    void Viewer::endGUI() { gui_window->end(); }

    void Viewer::notifyBecameVisible() {
        is_visible = true;
    }

    void Viewer::notifyBecameInvisible() {
        is_visible = false;
    }

    bool Viewer::isVisible() const {
        return is_visible;
    }


} // Studios::Fields::Viewers