//
// Created by joao on 10/23/24.
//

#ifndef STUDIOSLAB_IMGUINODESTEST3_H
#define STUDIOSLAB_IMGUINODESTEST3_H

#include "Graphics/Window/SlabWindow.h"
#include "3rdParty/ImGui.h"

namespace Tests {

    struct Link {
        int id;
        int start_attr;  // Output attribute ID
        int end_attr;    // Input attribute ID
    };

    struct Node {
        int id;
        std::string name;
        std::vector<int> input_ids;
        std::vector<int> output_ids;
    };

    namespace ed = ax::NodeEditor;

    class ImGuiNodesTest3 : public Slab::Graphics::SlabWindow {
        std::unordered_map<int, Node> nodes;
        std::vector<Link> links;
        ax::NodeEditor::EditorContext *m_Context;
    public:
        ImGuiNodesTest3();
        ~ImGuiNodesTest3() override;

        void draw() override;
    };

} // Tests

#endif //STUDIOSLAB_IMGUINODESTEST3_H
