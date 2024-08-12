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

    class MainViewer : public WindowRow {
        Pointer<GUIWindow> gui_window = New<GUIWindow>();

        Vector<Pointer<Viewer>> viewers;
        Pointer<Viewer> current_viewer;

        Pointer<Math::R2toR::NumericFunction> base_function;

    protected:
        virtual bool setCurrentViewer(Index i);

        auto getCurrentViewer() -> Pointer<Viewer>;

    public:
        explicit MainViewer(Pointer<Math::R2toR::NumericFunction> baseFunction=nullptr);

        void draw() override;

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        void setFunction(Pointer<Math::R2toR::NumericFunction>);

        auto getCurrentViewer() const -> Pointer<const Viewer>;

        auto getGUIWindow() -> Pointer<GUIWindow>;

        void addViewer(const Pointer<Viewer>&);
    };

} // Studios::Viewers

#endif //STUDIOSLAB_MAINVIEWER_H
