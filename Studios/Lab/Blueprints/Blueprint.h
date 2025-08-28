//
// Created by joao on 11/2/24.
//

#ifndef STUDIOSLAB_BLUEPRINT_H
#define STUDIOSLAB_BLUEPRINT_H

#include "BlueprintTypes.h"
#include "Utils/Map.h"
#include "Utils/Threads.h"

namespace Slab::Core
{
    class FInterface;
}

namespace Lab::Blueprints {

    class FBlueprint {
    public:
        using FNodeSpawner = std::function<void(FBlueprintNode&)>;

        FBlueprint();

        static int GetNextId()
        {
            return m_NextId++;
        }

        static Editor::LinkId GetNextLinkId()
        {
            return {static_cast<unsigned long>(GetNextId())};
        }

        auto GetNodes() -> Slab::Vector<FBlueprintNode>&;
        auto GetLinks() -> Slab::Vector<Link>&;

        FBlueprintNode* FindNode(Editor::NodeId id);

        Link* FindLink(Editor::LinkId id);

        Pin* FindPin(Editor::PinId id);

        bool IsPinLinked(Editor::PinId id) const;

        static
        bool CanCreateLink(Pin* a, Pin* b);

        bool CreateLink(Pin&a, Pin&b);

        static
        void BuildNode(FBlueprintNode* node);

        void RegisterSpawner(const Slab::Str& NodeClass, const FNodeSpawner& Spawner);

        FBlueprintNode* SpawnNode(const Slab::Str& NodeClass);

        FBlueprintNode* SpawnNodeFromInterface(Slab::Core::FInterface&);

        // FBlueprintNode* SpawnInputActionNode();

        FBlueprintNode* SpawnBranchNode();

        FBlueprintNode* SpawnDoNNode();

        FBlueprintNode* SpawnOutputActionNode();

        FBlueprintNode* SpawnPrintStringNode();

        FBlueprintNode* SpawnMessageNode();

        FBlueprintNode* SpawnSetTimerNode();

        FBlueprintNode* SpawnLessNode();

        FBlueprintNode* SpawnWeirdNode();

        FBlueprintNode* SpawnTraceByChannelNode();

        FBlueprintNode* SpawnTreeSequenceNode();

        FBlueprintNode* SpawnTreeTaskNode();

        FBlueprintNode* SpawnTreeTask2Node();

        FBlueprintNode* SpawnComment();

        FBlueprintNode* SpawnHoudiniTransformNode();

        FBlueprintNode* SpawnHoudiniGroupNode();

        void BuildNodes();

    private:
        Slab::Map<Slab::Str, FNodeSpawner> m_NodeSpawners;
        static Slab::Atomic<int>     m_NextId;
        const int              m_PinIconSize = 24;
        Slab::Vector<FBlueprintNode> m_Nodes;
        Slab::Vector<Link>    m_Links;
    };

} // Slab::Prototype

#endif //STUDIOSLAB_BLUEPRINT_H
