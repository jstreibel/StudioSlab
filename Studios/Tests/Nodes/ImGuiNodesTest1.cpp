//
// Created by joao on 10/23/24.
//

#include "ImGuiNodesTest1.h"

namespace Tests {
    namespace ed = ax::NodeEditor;

    ImGuiNodesTest1::ImGuiNodesTest1()
    : Slab::Graphics::SlabWindow({"Nodes", Slab::Graphics::RectI{1200, 2600, 200, 1000}}) {
        ed::Config config;
        config.SettingsFile = "Simple.json";
        m_Context = ed::CreateEditor(&config);
    }

    ImGuiNodesTest1::~ImGuiNodesTest1() {
        ed::DestroyEditor(m_Context);
    }

    void ImGuiNodesTest1::draw() {
        SlabWindow::draw();

        auto& io = ImGui::GetIO();

        if(ImGui::Begin("Nodes")) {

            ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

            ImGui::Separator();

            ed::SetCurrentEditor(m_Context);
            ed::Begin("My Editor", ImVec2(0.0, 0.0f));
            int uniqueId = 1;

            // Start drawing nodes.
            ed::BeginNode(uniqueId++);
            ImGui::Text("Node A");
            ed::BeginPin(uniqueId++, ed::PinKind::Input);
            ImGui::Text("-> In");
            ed::EndPin();
            ImGui::SameLine();
            ed::BeginPin(uniqueId++, ed::PinKind::Output);
            ImGui::Text("Out ->");
            ed::EndPin();
            ed::EndNode();
            ed::End();
            ed::SetCurrentEditor(nullptr);
        }
        ImGui::End();
    }
} // Tests