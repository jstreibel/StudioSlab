//
// Created by joao on 10/23/24.
//

#include "ImGuiNodesTest2.h"

#include "Core/SlabCore.h"
#include "Core/Tools/Resources.h"

#include "Graphics/Modules/ImGui/ImGuiModule.h"

namespace Tests::Blueprints {


    namespace ed = ax::NodeEditor;

    ImGuiNodesTest2::ImGuiNodesTest2()
    : Slab::Graphics::SlabWindow({"Nodes 2", Slab::Graphics::RectI{1300, 2700, 300, 1100}})
    {
        // m_Context = ed::CreateEditor();
        auto ImGuiModule = Slab::DynamicPointerCast<Slab::Graphics::ImGuiModule>(Slab::Core::GetModule("ImGui"));
        m_Context = ImGuiModule->getContext();

        /***** "RecreateFontAtlas" ******/
        {
            ImGuiIO &io = ImGui::GetIO();

            // IM_DELETE(io.Fonts);

            // io.Fonts = IM_NEW(ImFontAtlas);

            ImFontConfig config;
            config.OversampleH = 4;
            config.OversampleV = 4;
            config.PixelSnapH = false;

            auto fonts_folder = Slab::Core::Resources::FontsFolder + "Blueprints/";
            io.Fonts->AddFontFromFileTTF((fonts_folder + "Play-Regular.ttf").c_str(), 18.0f, &config);
            io.Fonts->AddFontFromFileTTF((fonts_folder + "Cuprum-Bold.ttf").c_str(), 20.0f, &config);

            // io.Fonts->Build();
        }



        // Blueprints "OnStart()"
        ed::Config config;

        config.SettingsFile = "Blueprints.json";

        config.UserPointer = this;

        config.LoadNodeSettings = [](ed::NodeId nodeId, char* data, void* userPointer) -> size_t
        {
            auto self = static_cast<ImGuiNodesTest2*>(userPointer);

            auto node = self->FindNode(nodeId);
            if (!node)
                return 0;

            if (data != nullptr)
                memcpy(data, node->State.data(), node->State.size());
            return node->State.size();
        };

        config.SaveNodeSettings = [](ed::NodeId nodeId, const char* data, size_t size, ed::SaveReasonFlags reason, void* userPointer) -> bool
        {
            auto self = static_cast<ImGuiNodesTest2*>(userPointer);

            auto node = self->FindNode(nodeId);
            if (!node)
                return false;

            node->State.assign(data, size);

            self->TouchNode(nodeId);

            return true;
        };

        m_Editor = ed::CreateEditor(&config);
        ed::SetCurrentEditor(m_Editor);

        Node* node;
        node = SpawnInputActionNode();      ed::SetNodePosition(node->ID, ImVec2(-252, 220));
        node = SpawnBranchNode();           ed::SetNodePosition(node->ID, ImVec2(-300, 351));
        node = SpawnDoNNode();              ed::SetNodePosition(node->ID, ImVec2(-238, 504));
        node = SpawnOutputActionNode();     ed::SetNodePosition(node->ID, ImVec2(71, 80));
        node = SpawnSetTimerNode();         ed::SetNodePosition(node->ID, ImVec2(168, 316));

        node = SpawnTreeSequenceNode();     ed::SetNodePosition(node->ID, ImVec2(1028, 329));
        node = SpawnTreeTaskNode();         ed::SetNodePosition(node->ID, ImVec2(1204, 458));
        node = SpawnTreeTask2Node();        ed::SetNodePosition(node->ID, ImVec2(868, 538));

        node = SpawnComment();              ed::SetNodePosition(node->ID, ImVec2(112, 576)); ed::SetGroupSize(node->ID, ImVec2(384, 154));
        node = SpawnComment();              ed::SetNodePosition(node->ID, ImVec2(800, 224)); ed::SetGroupSize(node->ID, ImVec2(640, 400));

        node = SpawnLessNode();             ed::SetNodePosition(node->ID, ImVec2(366, 652));
        node = SpawnWeirdNode();            ed::SetNodePosition(node->ID, ImVec2(144, 652));
        node = SpawnMessageNode();          ed::SetNodePosition(node->ID, ImVec2(-348, 698));
        node = SpawnPrintStringNode();      ed::SetNodePosition(node->ID, ImVec2(-69, 652));

        node = SpawnHoudiniTransformNode(); ed::SetNodePosition(node->ID, ImVec2(500, -70));
        node = SpawnHoudiniGroupNode();     ed::SetNodePosition(node->ID, ImVec2(500, 42));

        ed::NavigateToContent();

        BuildNodes();

        m_Links.emplace_back(GetNextLinkId(), m_Nodes[5].Outputs[0].ID, m_Nodes[6].Inputs[0].ID);
        m_Links.emplace_back(GetNextLinkId(), m_Nodes[5].Outputs[0].ID, m_Nodes[7].Inputs[0].ID);

        m_Links.emplace_back(GetNextLinkId(), m_Nodes[14].Outputs[0].ID, m_Nodes[15].Inputs[0].ID);

        auto location = Slab::Core::Resources::Folder + "Blueprints/";
        m_HeaderBackground = Slab::Graphics::LoadTexture(location + "BlueprintBackground.png");
        m_SaveIcon         = Slab::Graphics::LoadTexture(location + "ic_save_white_24dp.png");
        m_RestoreIcon      = Slab::Graphics::LoadTexture(location + "ic_restore_white_24dp.png");


        //auto& io = ImGui::GetIO();
    }

    ImGuiNodesTest2::~ImGuiNodesTest2() {
        auto releaseTexture = [this](ImTextureID& id)
        {
            if (id)
            {
                Slab::Graphics::DestroyTexture(id);
                id = nullptr;
            }

            // m_Renderer->Destroy();

            // m_Platform->ApplicationStop();

            if (m_Context)
            {
                ImGui::DestroyContext(m_Context);
                m_Context= nullptr;
            }
        };

        // releaseTexture(m_RestoreIcon);
        // releaseTexture(m_SaveIcon);
        // releaseTexture(m_HeaderBackground);

        if (m_Editor)
        {
            ed::DestroyEditor(m_Editor);
            m_Editor = nullptr;
        }
    }

} // Tests