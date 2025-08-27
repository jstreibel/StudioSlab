//
// Created by joao on 11/2/24.
//

#ifndef STUDIOSLAB_BLUEPRINT_H
#define STUDIOSLAB_BLUEPRINT_H

#include "BlueprintTypes.h"

namespace Slab::Blueprints {

    class FBlueprint {
        int                  m_NextId = 1;
        const int            m_PinIconSize = 24;
        Vector<FBlueprintNode>    m_Nodes;
        Vector<Link>    m_Links;

    public:
        int GetNextId()
        {
            return m_NextId++;
        }

        ed::LinkId GetNextLinkId()
        {
            return {static_cast<unsigned long>(GetNextId())};
        }

        auto GetNodes() -> Vector<FBlueprintNode>&;
        auto GetLinks() -> Vector<Link>&;

        FBlueprintNode* FindNode(ed::NodeId id);

        Link* FindLink(ed::LinkId id);

        Pin* FindPin(ed::PinId id);

        bool IsPinLinked(ed::PinId id);

        bool CanCreateLink(Pin* a, Pin* b);

        bool CreateLink(Pin&a, Pin&b);

        void BuildNode(FBlueprintNode* node);

        FBlueprintNode* SpawnInputActionNode();

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
    };

} // Slab::Prototype

#endif //STUDIOSLAB_BLUEPRINT_H
