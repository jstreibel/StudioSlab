//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_VIEWER_H
#define STUDIOSLAB_VIEWER_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Graphics/Window/GUIWindow.h"

namespace Studios::Fields::Viewers {

    class Viewer : public Slab::Graphics::WindowPanel {
        Slab::Pointer<Slab::Graphics::GUIWindow> gui_window;
        Slab::Pointer<Slab::Math::R2toR::NumericFunction> base_function;

    protected:
        void beginGUI();
        void endGUI();

    public:
        Viewer(Slab::Pointer<Slab::Graphics::GUIWindow>, Slab::Pointer<Slab::Math::R2toR::NumericFunction> = nullptr);

        virtual void setFunction(Slab::Pointer<Slab::Math::R2toR::NumericFunction> function);
        auto getFunction() -> Slab::Pointer<Slab::Math::R2toR::NumericFunction>;

        virtual void notifyBecameVisible();
        virtual void notifyBecameInvisible();
    };

} // Studios::Fields::Viewers

#endif //STUDIOSLAB_VIEWER_H
