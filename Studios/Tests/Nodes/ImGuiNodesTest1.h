//
// Created by joao on 10/23/24.
//

#ifndef STUDIOSLAB_IMGUINODESTEST1_H
#define STUDIOSLAB_IMGUINODESTEST1_H

#include "Graphics/Window/SlabWindow.h"

#include "3rdParty/ImGui.h"

namespace Tests {

    class ImGuiNodesTest1 : public Slab::Graphics::SlabWindow {
        ax::NodeEditor::EditorContext *m_Context;
    public:
        ImGuiNodesTest1();
        ~ImGuiNodesTest1() override;

        void draw() override;
    };

} // Slab

#endif //STUDIOSLAB_IMGUINODESTEST1_H
