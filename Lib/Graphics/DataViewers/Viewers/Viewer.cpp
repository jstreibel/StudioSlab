//
// Created by joao on 7/9/24.
//

#include "Viewer.h"

#include <utility>

namespace Slab::Graphics {

    Viewer::Viewer(Pointer<FGUIWindow> gui_window, Pointer<Math::R2toR::FNumericFunction> func)
    : Graphics::WindowPanel(FConfig{{}})
    , gui_window(std::move(gui_window))
    , base_function(std::move(func))
    {}

    void Viewer::SetFunction(Pointer<Math::R2toR::FNumericFunction> function) {
        base_function = std::move(function);
    }

    auto Viewer::getFunction() const -> Pointer<const Math::R2toR::FNumericFunction> {
        return base_function;
    }

    void Viewer::NotifyBecameVisible() {
        is_visible = true;
    }

    void Viewer::notifyBecameInvisible() {
        is_visible = false;
    }

    bool Viewer::isVisible() const {
        return is_visible;
    }

} // Studios::Fields::Viewers