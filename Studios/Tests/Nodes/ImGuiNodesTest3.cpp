//
// Created by joao on 10/23/24.
//

#include "ImGuiNodesTest3.h"

namespace Tests {

    namespace ed = ax::NodeEditor;


    ImGuiNodesTest3::ImGuiNodesTest3()
            : Slab::Graphics::SlabWindow({"Nodes", Slab::Graphics::RectI{1200, 2600, 200, 1000}}) {
        m_Context = ed::CreateEditor();

        // Initialize some nodes
        nodes[1] = {1, "Node A", {101}, {201}};
        nodes[2] = {2, "Node B", {102}, {202}};
        nodes[3] = {3, "Node C", {103}, {203}};

        // Initialize some links
        links.push_back({1, 201, 102});  // Link from Node A output to Node B input
        links.push_back({2, 202, 103});  // Link from Node B output to Node C input
    }

    ImGuiNodesTest3::~ImGuiNodesTest3() {
        ed::DestroyEditor(m_Context);
    }

    void ImGuiNodesTest3::draw() {
        SlabWindow::draw();

        ed::SetCurrentEditor(m_Context);

        if(ImGui::Begin("Nodes")) {
            ed::Begin("Flow Pipeline Editor");
            for (auto &[id, node]: nodes) {
                ed::BeginNode(id);

                ImGui::Text("%s", node.name.c_str());

                // Draw Input Slots
                for (int input: node.input_ids) {
                    ed::BeginPin(input, ed::PinKind::Input);
                    ImGui::Text("Input %d", input);
                    ed::EndPin();
                }

                // Draw Output Slots
                for (int output: node.output_ids) {
                    ed::BeginPin(output, ed::PinKind::Output);
                    ImGui::Text("Output %d", output);
                    ed::EndPin();
                }

                ed::EndNode();
            }

            // Draw the links
            for (const auto &link: links) {
                ed::Link(link.id, link.start_attr, link.end_attr);
            }

            ed::End();

            ed::SetCurrentEditor(nullptr);
        }
        ImGui::End();
    }

} // Tests