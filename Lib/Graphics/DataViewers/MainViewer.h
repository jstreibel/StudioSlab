//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_MAINVIEWER_H
#define STUDIOSLAB_MAINVIEWER_H

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Window/WindowProxy.h"
#include "Graphics/Window/WindowContainer/WindowRow.h"

#include "Graphics/DataViewers/Viewers/Viewer.h"

namespace Slab::Graphics {

    class MainViewer : public FWindowRow {
        Pointer<FGUIWindow> gui_window;

        Vector<Pointer<Viewer>> viewers;
        Pointer<Viewer> current_viewer;

        Pointer<Math::R2toR::FNumericFunction> base_function;

    protected:
        virtual bool setCurrentViewer(Index i);

        auto getCurrentViewer() -> Pointer<Viewer>;

    public:
        explicit MainViewer(Pointer<Math::R2toR::FNumericFunction> baseFunction=nullptr);

        void ImmediateDraw(const FPlatformWindow&) override;

        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;

        void setFunction(Pointer<Math::R2toR::FNumericFunction>);

        auto getCurrentViewer() const -> Pointer<const Viewer>;

        auto getGUIWindow() -> Pointer<FGUIWindow>;

        void addViewer(const Pointer<Viewer>&);
    };

} // Studios::Viewers

#endif //STUDIOSLAB_MAINVIEWER_H
