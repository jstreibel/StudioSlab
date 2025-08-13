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
        TPointer<Math::R2toR::FNumericFunction> base_function;

        bool is_visible = false;

    protected:
        TPointer<FGUIWindow> gui_window;

    public:
        explicit Viewer(TPointer<FGUIWindow>, Slab::TPointer<Math::R2toR::FNumericFunction> = nullptr);

        virtual void SetFunction(TPointer<Math::R2toR::FNumericFunction> function);
        auto getFunction() const -> TPointer<const Math::R2toR::FNumericFunction>;

        virtual Str GetName() const = 0;

        virtual void NotifyBecameVisible();
        virtual void notifyBecameInvisible();
        bool isVisible() const;
    };

} // Studios::Fields::Viewers

#endif //STUDIOSLAB_VIEWER_H
