//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_VIEWER_H
#define STUDIOSLAB_VIEWER_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Graphics/Window/GUIWindow.h"

namespace Slab::Graphics {

    class Viewer : public WindowPanel {
        Pointer<Math::R2toR::FNumericFunction> base_function;

        bool is_visible = false;

    protected:
        Pointer<FGUIWindow> gui_window;

    public:
        explicit Viewer(Pointer<FGUIWindow>, Slab::Pointer<Math::R2toR::FNumericFunction> = nullptr);

        virtual void SetFunction(Pointer<Math::R2toR::FNumericFunction> function);
        auto getFunction() const -> Pointer<const Math::R2toR::FNumericFunction>;

        virtual Str GetName() const = 0;

        virtual void NotifyBecameVisible();
        virtual void notifyBecameInvisible();
        bool isVisible() const;
    };

} // Studios::Fields::Viewers

#endif //STUDIOSLAB_VIEWER_H
