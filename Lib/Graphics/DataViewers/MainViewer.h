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
        TPointer<FGUIWindow> gui_window;

        Vector<TPointer<Viewer>> viewers;
        TPointer<Viewer> current_viewer;

        TPointer<Math::R2toR::FNumericFunction> base_function;

    protected:
        virtual bool setCurrentViewer(Index i);

        auto getCurrentViewer() -> TPointer<Viewer>;

    public:
        explicit MainViewer(TPointer<Math::R2toR::FNumericFunction> baseFunction=nullptr);

        void ImmediateDraw(const FPlatformWindow&) override;

        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;

        void setFunction(TPointer<Math::R2toR::FNumericFunction>);

        auto getCurrentViewer() const -> TPointer<const Viewer>;

        auto getGUIWindow() -> TPointer<FGUIWindow>;

        void addViewer(const TPointer<Viewer>&);
    };

} // Studios::Viewers

#endif //STUDIOSLAB_MAINVIEWER_H
