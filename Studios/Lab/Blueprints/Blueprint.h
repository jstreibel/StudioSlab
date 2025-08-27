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

namespace Slab::Blueprints {

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

        auto GetNodes() -> Vector<FBlueprintNode>&;
        auto GetLinks() -> Vector<Link>&;

        FBlueprintNode* FindNode(Editor::NodeId id);

        Link* FindLink(Editor::LinkId id);

        Pin* FindPin(Editor::PinId id);

        bool IsPinLinked(Editor::PinId id) const;

        static
        bool CanCreateLink(Pin* a, Pin* b);

        bool CreateLink(Pin&a, Pin&b);

        static
        void BuildNode(FBlueprintNode* node);

        void RegisterSpawner(const Str& NodeClass, const FNodeSpawner& Spawner);

        FBlueprintNode* SpawnNode(const Str& NodeClass);

        FBlueprintNode* SpawnNodeFromInterface(Core::FInterface&);

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
        Map<Str, FNodeSpawner> m_NodeSpawners;
        static Atomic<int>     m_NextId;
        const int              m_PinIconSize = 24;
        Vector<FBlueprintNode> m_Nodes;
        Vector<Link>    m_Links;
    };

} // Slab::Prototype

#endif //STUDIOSLAB_BLUEPRINT_H
