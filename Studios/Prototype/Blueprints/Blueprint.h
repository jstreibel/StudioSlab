//
// Created by joao on 11/2/24.
//

#ifndef STUDIOSLAB_BLUEPRINT_H
#define STUDIOSLAB_BLUEPRINT_H

#include "BlueprintTypes.h"

namespace Slab::Blueprints {

    class Blueprint {
        int                  m_NextId = 1;
        const int            m_PinIconSize = 24;
        Vector<Node>    m_Nodes;
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

        Vector<Node> GetNodes();

        Vector<Link> GetLinks();

        Node* FindNode(ed::NodeId id);

        Link* FindLink(ed::LinkId id);

        Pin* FindPin(ed::PinId id);

        bool IsPinLinked(ed::PinId id);

        bool CanCreateLink(Pin* a, Pin* b);

        bool CreateLink(Pin&a, Pin&b);

        void BuildNode(Node* node);

        Node* SpawnInputActionNode();

        Node* SpawnBranchNode();

        Node* SpawnDoNNode();

        Node* SpawnOutputActionNode();

        Node* SpawnPrintStringNode();

        Node* SpawnMessageNode();

        Node* SpawnSetTimerNode();

        Node* SpawnLessNode();

        Node* SpawnWeirdNode();

        Node* SpawnTraceByChannelNode();

        Node* SpawnTreeSequenceNode();

        Node* SpawnTreeTaskNode();

        Node* SpawnTreeTask2Node();

        Node* SpawnComment();

        Node* SpawnHoudiniTransformNode();

        Node* SpawnHoudiniGroupNode();

        void BuildNodes();
    };

} // Slab::Prototype

#endif //STUDIOSLAB_BLUEPRINT_H
