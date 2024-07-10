//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_MAINVIEWER_H
#define STUDIOSLAB_MAINVIEWER_H

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Window/WindowProxy.h"
#include "Graphics/Window/WindowContainer/WindowRow.h"

#include "Viewer.h"

namespace Studios::Fields::Viewers {
    using namespace Slab;

    class MainViewer : public Graphics::WindowRow {
        Pointer<Graphics::GUIWindow> gui_window = New<Graphics::GUIWindow>();

        Vector<Pointer<Viewer>> viewers;
        Pointer<Viewer> current_viewer;

        Pointer<Math::R2toR::NumericFunction> base_function;

    protected:
        virtual bool setCurrentViewer(Index i);

    public:
        explicit MainViewer(Pointer<Math::R2toR::NumericFunction> baseFunction=nullptr);

        void draw() override;

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        void setFunction(Pointer<Math::R2toR::NumericFunction>);

        auto getCurrentViewer() const -> Pointer<const Viewer>;

        auto getGUIWindow() -> Pointer<Graphics::GUIWindow>;

        void addViewer(Pointer<Viewer>);
    };

} // Studios::Viewers

#endif //STUDIOSLAB_MAINVIEWER_H
