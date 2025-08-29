//
// Created by joao on 11/2/24.
//

#include "Blueprint.h"

#include "Core/Controller/Interface.h"

using namespace Slab;

namespace Lab::Blueprints {
    Atomic<int> FBlueprint::m_NextId(1);

    FBlueprint::FBlueprint() = default;

    auto FBlueprint::GetNodes() -> Vector<FBlueprintNode>& { return m_Nodes; }

    auto FBlueprint::GetLinks() -> Vector<Link>& { return m_Links; }

    FBlueprintNode *FBlueprint::FindNode(Editor::NodeId id) {
        for (auto& node : m_Nodes) if (node.ID == id) return &node;

        return nullptr;
    }

    Link *FBlueprint::FindLink(Editor::LinkId id) {
        for (auto& link : m_Links) if (link.ID == id) return &link;

        return nullptr;
    }

    Pin *FBlueprint::FindPin(Editor::PinId id) {
        if (!id) return nullptr;

        for (auto& node : m_Nodes)
        {
            for (auto& pin : node.Inputs) if (pin.ID == id) return &pin;

            for (auto& pin : node.Outputs) if (pin.ID == id) return &pin;
        }

        return nullptr;
    }

    bool FBlueprint::IsPinLinked(Editor::PinId id) const
    {
        if (!id) return false;

        for (auto& link : m_Links)
            if (link.StartPinID == id || link.EndPinID == id)
                return true;

        return false;
    }

    FBlueprint::FNodeSpawnerMap FBlueprint::GetNodeSpawners()
    {
        return m_NodeSpawners;
    }

    bool FBlueprint::CanCreateLink(Pin *a, Pin *b) {
        if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
            return false;

        return true;
    }

    bool FBlueprint::CreateLink(Pin &a, Pin &b) {
        if(!CanCreateLink(&a, &b)) return false;

        m_Links.emplace_back(GetNextLinkId(), a.ID, b.ID);

        return true;
    }

    void FBlueprint::BuildNode(FBlueprintNode *node) {
        for (auto& input : node->Inputs)
        {
            input.Node = node;
            input.Kind = PinKind::Input;
        }

        for (auto& output : node->Outputs)
        {
            output.Node = node;
            output.Kind = PinKind::Output;
        }
    }

    void FBlueprint::RegisterSpawner(const Str& NodeClass, const FNodeSpawner& Spawner)
    {
        m_NodeSpawners[NodeClass] = Spawner;
    }

    FBlueprintNode* FBlueprint::SpawnNode(const Str& NodeClass)
    {
        const auto It = m_NodeSpawners.find(NodeClass);
        if (It == m_NodeSpawners.end()) return nullptr;

        const auto Spawner = It->second;

        auto Id = GetNextId();
        fix GenericName = "Node " + ToStr(Id);
        m_Nodes.emplace_back(Id, NodeClass.c_str(), ImColor(255, 128, 128));

        auto &Node = m_Nodes.back();
        Spawner(Node);

        BuildNode(&Node);

        return &Node;
    }

    FBlueprintNode* FBlueprint::SpawnNodeFromInterface(Core::FInterface& Interface)
    {
        fix Name = Interface.GetName();

        m_Nodes.emplace_back(GetNextId(), Name.c_str(), ImColor(255, 128, 128));
        auto &Node = m_Nodes.back();

        for (auto &Parameter : Interface.GetParameters())
        {
            auto Type = PinType::Float;
            switch (Parameter->GetType())
            {
            case Core::EParameterType::ParameterType_Integer: Type = PinType::Int; break;
            case Core::EParameterType::ParameterType_Bool: Type = PinType::Bool; break;
            case Core::EParameterType::ParameterType_String: Type = PinType::String; break;
            case Core::EParameterType::ParameterType_Float: Type = PinType::Float; break;
            case Core::EParameterType::ParameterType_MultiString: Type = PinType::Delegate; break;
            case Core::EParameterType::ParameterType_Uninformed: Type = PinType::Object; break;
            }
            Node.Inputs.emplace_back(GetNextId(), Parameter->GetName().c_str(), Type);
        }

        for (const auto &SubInterface : Interface.GetSubInterfaces())
        {
            Node.Inputs.emplace_back(GetNextId(), SubInterface->GetName().c_str(), PinType::Function);
        }

        BuildNode(&Node);

        return &Node;
    }

    void FBlueprint::BuildNodes() {
        for (auto& node : m_Nodes)
            BuildNode(&node);
    }

    void FBlueprint::SetupDemo()
    {
        RegisterSpawner("Input Action", [](FBlueprintNode& Node)
        {
            Node.Name = "InputAction Fire";
            Node.Color = ImColor(255, 128, 128);

            Node.Outputs.emplace_back(GetNextId(), "",         PinType::Delegate);
            Node.Outputs.emplace_back(GetNextId(), "Pressed",  PinType::Flow);
            Node.Outputs.emplace_back(GetNextId(), "Released", PinType::Flow);

        });
        RegisterSpawner("Branch", [](FBlueprintNode& Node)
        {
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            Node.Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
            Node.Outputs.emplace_back(GetNextId(), "True", PinType::Flow);
            Node.Outputs.emplace_back(GetNextId(), "False", PinType::Flow);
        });
        RegisterSpawner("Do N", [](FBlueprintNode& Node)
        {
            Node.Inputs.emplace_back(GetNextId(), "Enter", PinType::Flow);
            Node.Inputs.emplace_back(GetNextId(), "N", PinType::Int);
            Node.Inputs.emplace_back(GetNextId(), "Reset", PinType::Flow);
            Node.Outputs.emplace_back(GetNextId(), "Exit", PinType::Flow);
            Node.Outputs.emplace_back(GetNextId(), "Counter", PinType::Int);
        });
        RegisterSpawner("Output Action", [](FBlueprintNode& Node)
        {
            Node.Name = "OutputAction";
            Node.Inputs.emplace_back(GetNextId(), "Sample", PinType::Float);
            Node.Outputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
            Node.Inputs.emplace_back(GetNextId(), "Event", PinType::Delegate);
        });
        RegisterSpawner("Print String", [](FBlueprintNode& Node)
        {
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            Node.Inputs.emplace_back(GetNextId(), "In String", PinType::String);
            Node.Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        });
        RegisterSpawner("Message",  [](FBlueprintNode& Node)
        {
            Node.Name = "";
            Node.Color = ImColor(128, 195, 248);
            Node.Type = NodeType::Simple;
            Node.Outputs.emplace_back(GetNextId(), "Message", PinType::String);
        });
        RegisterSpawner("Set Timer", [](FBlueprintNode& Node)
        {
            Node.Color = ImColor(128, 195, 248);
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            Node.Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
            Node.Inputs.emplace_back(GetNextId(), "Function Name", PinType::Function);
            Node.Inputs.emplace_back(GetNextId(), "Time", PinType::Float);
            Node.Inputs.emplace_back(GetNextId(), "Looping", PinType::Bool);
            Node.Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        });
        RegisterSpawner("Less", [](FBlueprintNode& Node)
        {
            Node.Name = "<";
            Node.Color = ImColor(128, 195, 248);
            Node.Type = NodeType::Simple;
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Float);
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Float);
            Node.Outputs.emplace_back(GetNextId(), "", PinType::Float);
        });
        RegisterSpawner("Weird", [](FBlueprintNode& Node)
        {
            Node.Name = "o.O";
            Node.Color = ImColor(128, 195, 248);
            Node.Type = NodeType::Simple;
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Bool);
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Float);
            Node.Outputs.emplace_back(GetNextId(), "", PinType::Delegate);
        });
        RegisterSpawner("Trace by Channel", [](FBlueprintNode& Node)
        {
            Node.Name = "Single Line Trace by Channel";
            Node.Color = ImColor(255, 128, 64);

            Node.Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            Node.Inputs.emplace_back(GetNextId(), "Start", PinType::Flow);
            Node.Inputs.emplace_back(GetNextId(), "End", PinType::Int);
            Node.Inputs.emplace_back(GetNextId(), "Trace Channel", PinType::Float);
            Node.Inputs.emplace_back(GetNextId(), "Trace Complex", PinType::Bool);
            Node.Inputs.emplace_back(GetNextId(), "Actors to Ignore", PinType::Int);
            Node.Inputs.emplace_back(GetNextId(), "Draw Debug Type", PinType::Bool);
            Node.Inputs.emplace_back(GetNextId(), "Ignore Self", PinType::Bool);
            Node.Outputs.emplace_back(GetNextId(), "", PinType::Flow);
            Node.Outputs.emplace_back(GetNextId(), "Out Hit", PinType::Float);
            Node.Outputs.emplace_back(GetNextId(), "Return Value", PinType::Bool);
        });
        RegisterSpawner("Tree Sequence", [](FBlueprintNode& Node)
        {
            Node.Name = "Sequence";
            Node.Type = NodeType::Tree;
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            Node.Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        });
        RegisterSpawner("Tree Task", [](FBlueprintNode& Node)
        {
            Node.Name = "Move To";
            Node.Type = NodeType::Tree;
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        });
        RegisterSpawner("Random Wait", [](FBlueprintNode& Node)
        {
            Node.Type = NodeType::Tree;
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        });
        RegisterSpawner("Test Comment", [](FBlueprintNode& Node)
        {
            Node.Type = NodeType::Comment;
            Node.Size = ImVec2(300, 200);
        });
        RegisterSpawner("Houdini Transform", [](FBlueprintNode& Node)
        {
            Node.Name = "Transform";
            Node.Type = NodeType::Houdini;
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            Node.Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        });
        RegisterSpawner("Houdini Group", [](FBlueprintNode& Node)
        {
            Node.Name = "Group";
            Node.Type = NodeType::Houdini;

            Node.Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            Node.Inputs.emplace_back(GetNextId(), "", PinType::Flow);
            Node.Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        });

        const FBlueprintNode* Node = nullptr;

        Node = SpawnNode("Input Action");     if (Node != nullptr) Editor::SetNodePosition(Node->ID, ImVec2(-252, 220));
        Node = SpawnNode("Branch");           if (Node != nullptr) Editor::SetNodePosition(Node->ID, ImVec2(-300, 351));
        Node = SpawnNode("Do N");             if (Node != nullptr) Editor::SetNodePosition(Node->ID, ImVec2(-238, 504));
        Node = SpawnNode("Output Action");    if (Node != nullptr) Editor::SetNodePosition(Node->ID, ImVec2(71, 80));
        Node = SpawnNode("Set Timer");        if (Node != nullptr) Editor::SetNodePosition(Node->ID, ImVec2(168, 316));

        Node = SpawnNode("Tree Sequence");    if (Node != nullptr) Editor::SetNodePosition(Node->ID, ImVec2(1028, 329));
        Node = SpawnNode("Tree Task");        if (Node != nullptr) Editor::SetNodePosition(Node->ID, ImVec2(1204, 458));
        Node = SpawnNode("Random Wait");      if (Node != nullptr) Editor::SetNodePosition(Node->ID, ImVec2(868, 538));

        Node = SpawnNode("Test Comment");     if (Node != nullptr) Editor::SetNodePosition(Node->ID, ImVec2(112, 576)); Editor::SetGroupSize(Node->ID, ImVec2(384, 154));
        Node = SpawnNode("Test Comment");     if (Node != nullptr) Editor::SetNodePosition(Node->ID, ImVec2(800, 224)); Editor::SetGroupSize(Node->ID, ImVec2(640, 400));

        // Node = SpawnLessNode();             Editor::SetNodePosition(Node->ID, ImVec2(366, 652));
        // Node = SpawnWeirdNode();            Editor::SetNodePosition(Node->ID, ImVec2(144, 652));
        // Node = SpawnMessageNode();          Editor::SetNodePosition(Node->ID, ImVec2(-348, 698));
        // Node = SpawnPrintStringNode();      Editor::SetNodePosition(Node->ID, ImVec2(-69, 652));

        // Node = SpawnHoudiniTransformNode(); Editor::SetNodePosition(Node->ID, ImVec2(500, -70));
        // Node = SpawnHoudiniGroupNode();     Editor::SetNodePosition(Node->ID, ImVec2(500, 42));

        Editor::NavigateToContent();

        BuildNodes();

        // auto nodes = GetNodes();
        // CreateLink(nodes[5].Outputs[0], nodes[6].Outputs[0]);
        // CreateLink(nodes[5].Outputs[0], nodes[6].Inputs[0]);
        // CreateLink(nodes[5].Outputs[0], nodes[7].Inputs[0]);
        // CreateLink(nodes[14].Outputs[0], nodes[15].Inputs[0]);
    }
} // Slab::Prototype