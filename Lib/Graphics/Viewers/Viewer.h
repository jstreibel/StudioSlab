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
        Pointer<GUIWindow> gui_window;
        Pointer<Math::R2toR::NumericFunction> base_function;

    protected:
        void beginGUI();
        void endGUI();

    public:
        Viewer(Pointer<GUIWindow>, Slab::Pointer<Math::R2toR::NumericFunction> = nullptr);

        virtual void setFunction(Pointer<Math::R2toR::NumericFunction> function);
        auto getFunction() const -> Pointer<const Math::R2toR::NumericFunction>;

        virtual void notifyBecameVisible();
        virtual void notifyBecameInvisible();
    };

} // Studios::Fields::Viewers

#endif //STUDIOSLAB_VIEWER_H
