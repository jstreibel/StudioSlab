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

    class GUIWindow : public SlabWindow {
        Vector<Pair<Str, Color>> stats;

        Pointer<SlabImGuiContext> gui_context;

    public:
        explicit GUIWindow(Config={});

        void addVolatileStat(const Str &stat, Color color = {-1, -1, -1});

        void AddExternalDraw(const ExternalDraw&);

        void draw() override;
        void begin() const;
        void end() const;

    };


}

#endif //STUDIOSLAB_GUIWINDOW_H
