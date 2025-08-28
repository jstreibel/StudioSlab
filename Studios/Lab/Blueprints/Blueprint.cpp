//
// Created by joao on 11/2/24.
//

#include "Blueprint.h"

#include "Core/Controller/Interface.h"

namespace Slab::Blueprints {
    Atomic<int> FBlueprint::m_NextId(1);

    FBlueprint::FBlueprint()
    {
        RegisterSpawner("InputAction", [](FBlueprintNode& Node)
        {
            Node.Name = "InputAction Fire";
            Node.Color = ImColor(255, 128, 128);

            Node.Outputs.emplace_back(FBlueprint::GetNextId(), "",         PinType::Delegate);
            Node.Outputs.emplace_back(FBlueprint::GetNextId(), "Pressed",  PinType::Flow);
            Node.Outputs.emplace_back(FBlueprint::GetNextId(), "Released", PinType::Flow);

        });
    }

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
        m_Nodes.emplace_back(Id, ("Node " + ToStr(Id)).c_str(), ImColor(255, 128, 128));

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

    FBlueprintNode *FBlueprint::SpawnBranchNode() {
        m_Nodes.emplace_back(GetNextId(), "Branch");
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "True", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "False", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnDoNNode() {
        m_Nodes.emplace_back(GetNextId(), "Do N");
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Enter", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "N", PinType::Int);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Reset", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Exit", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Counter", PinType::Int);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnOutputActionNode() {
        m_Nodes.emplace_back(GetNextId(), "OutputAction");
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Sample", PinType::Float);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Event", PinType::Delegate);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnPrintStringNode() {
        m_Nodes.emplace_back(GetNextId(), "Print String");
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "In String", PinType::String);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnMessageNode() {
        m_Nodes.emplace_back(GetNextId(), "", ImColor(128, 195, 248));
        m_Nodes.back().Type = NodeType::Simple;
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Message", PinType::String);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnSetTimerNode() {
        m_Nodes.emplace_back(GetNextId(), "Set Timer", ImColor(128, 195, 248));
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Function Name", PinType::Function);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Time", PinType::Float);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Looping", PinType::Bool);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnLessNode() {
        m_Nodes.emplace_back(GetNextId(), "<", ImColor(128, 195, 248));
        m_Nodes.back().Type = NodeType::Simple;
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnWeirdNode() {
        m_Nodes.emplace_back(GetNextId(), "o.O", ImColor(128, 195, 248));
        m_Nodes.back().Type = NodeType::Simple;
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnTraceByChannelNode() {
        m_Nodes.emplace_back(GetNextId(), "Single Line Trace by Channel", ImColor(255, 128, 64));
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Start", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "End", PinType::Int);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Trace Channel", PinType::Float);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Trace Complex", PinType::Bool);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Actors to Ignore", PinType::Int);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Draw Debug Type", PinType::Bool);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "Ignore Self", PinType::Bool);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Out Hit", PinType::Float);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "Return Value", PinType::Bool);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnTreeSequenceNode() {
        m_Nodes.emplace_back(GetNextId(), "Sequence");
        m_Nodes.back().Type = NodeType::Tree;
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnTreeTaskNode() {
        m_Nodes.emplace_back(GetNextId(), "Move To");
        m_Nodes.back().Type = NodeType::Tree;
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnTreeTask2Node() {
        m_Nodes.emplace_back(GetNextId(), "Random Wait");
        m_Nodes.back().Type = NodeType::Tree;
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnComment() {
        m_Nodes.emplace_back(GetNextId(), "Test Comment");
        m_Nodes.back().Type = NodeType::Comment;
        m_Nodes.back().Size = ImVec2(300, 200);

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnHoudiniTransformNode() {
        m_Nodes.emplace_back(GetNextId(), "Transform");
        m_Nodes.back().Type = NodeType::Houdini;
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    FBlueprintNode *FBlueprint::SpawnHoudiniGroupNode() {
        m_Nodes.emplace_back(GetNextId(), "Group");
        m_Nodes.back().Type = NodeType::Houdini;
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        m_Nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

        BuildNode(&m_Nodes.back());

        return &m_Nodes.back();
    }

    void FBlueprint::BuildNodes() {
        for (auto& node : m_Nodes)
            BuildNode(&node);
    }

} // Slab::Prototype