//
// Created by joao on 8/8/22.
//

#ifndef STUDIOSLAB_GUIWINDOW_H
#define STUDIOSLAB_GUIWINDOW_H

#include "3rdParty/ImGui.h"

#include "Graphics/Utils.h"
#include "Graphics/Window/SlabWindow.h"

#include "Utils/Types.h"
#include "Graphics/Modules/ImGui/ImGuiContext.h"


namespace Slab::Graphics {

    class FGUIWindow : public FSlabWindow {
        Vector<Pair<Str, FColor>> Stats;

        Pointer<FImGuiContext> GuiContext;

        void Begin() const;
        void End() const;
    public:
        explicit FGUIWindow(FConfig);

        void AddVolatileStat(const Str &stat, FColor color = {-1, -1, -1});

        void AddExternalDraw(const FDrawCall&) const;

        Pointer<FImGuiContext> GetGUIContext();

        void ImmediateDraw() override;

        auto RegisterDeferredDrawCalls() -> void override;
    };


}

#endif //STUDIOSLAB_GUIWINDOW_H
