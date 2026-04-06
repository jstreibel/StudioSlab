//
// Created by joao on 7/9/24.
//

#include "Viewer.h"

#include <utility>

namespace Slab::Graphics {

    FViewer::FViewer(TPointer<FGUIWindow> gui_window, TPointer<Math::R2toR::FNumericFunction> func)
    : Graphics::FWindowPanel(FSlabWindowConfig{{}})
    , gui_window(std::move(gui_window))
    , base_function(std::move(func))
    {}

    void FViewer::SetFunction(TPointer<Math::R2toR::FNumericFunction> function) {
        base_function = std::move(function);
    }

    auto FViewer::getFunction() const -> TPointer<const Math::R2toR::FNumericFunction> {
        return base_function;
    }

    void FViewer::NotifyBecameVisible() {
        is_visible = true;
    }

    void FViewer::notifyBecameInvisible() {
        is_visible = false;
    }

    bool FViewer::isVisible() const {
        return is_visible;
    }

} // Studios::Fields::Viewers
