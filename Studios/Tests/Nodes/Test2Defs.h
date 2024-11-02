//
// Created by joao on 10/23/24.
//

#ifndef STUDIOSLAB_TEST2DEFS_H
#define STUDIOSLAB_TEST2DEFS_H

#include "3rdParty/ImGui.h"
#include "3rdParty/imgui/imgui_internal.h"

#include "utilities/drawing.h"
#include "utilities/builders.h"
#include "utilities/widgets.h"
#include "Utils/Numbers.h"

#include <map>

namespace Tests::Blueprints {
    static inline ImRect ImGui_GetItemRect()
    {
        return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
    }

    static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
    {
        auto result = rect;
        result.Min.x -= x;
        result.Min.y -= y;
        result.Max.x += x;
        result.Max.y += y;
        return result;
    }

    namespace ed = ax::NodeEditor;
    namespace util = ax::NodeEditor::Utilities;

    using namespace ax;

    using ax::Widgets::IconType;

    static ed::EditorContext* m_Editor = nullptr;

//extern "C" __declspec(dllimport) short __stdcall GetAsyncKeyState(int vkey);
//extern "C" bool Debug_KeyPress(int vkey)
//{
//    static std::map<int, bool> state;
//    auto lastState = state[vkey];
//    state[vkey] = (GetAsyncKeyState(vkey) & 0x8000) != 0;
//    if (state[vkey] && !lastState)
//        return true;
//    else
//        return false;
//}

    enum class PinType
    {
        Flow,
        Bool,
        Int,
        Float,
        String,
        Object,
        Function,
        Delegate,
    };

    enum class PinKind
    {
        Output,
        Input
    };

    enum class NodeType
    {
        Blueprint,
        Simple,
        Tree,
        Comment,
        Houdini
    };

    struct Node;

    struct Pin
    {
        ed::PinId   ID;
        Blueprints::Node*     Node;
        std::string Name;
        PinType     Type;
        PinKind     Kind;

        Pin(int id, const char* name, PinType type):
                ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input)
        {
        }
    };

    struct Node
    {
        ed::NodeId ID;
        std::string Name;
        std::vector<Pin> Inputs;
        std::vector<Pin> Outputs;
        ImColor Color;
        NodeType Type;
        ImVec2 Size;

        std::string State;
        std::string SavedState;

        Node(int id, const char* name, ImColor color = ImColor(255, 255, 255)):
                ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0)
        {
        }
    };

    struct Link
    {
        ed::LinkId ID;

        ed::PinId StartPinID;
        ed::PinId EndPinID;

        ImColor Color;

        Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId):
                ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
        {
        }
    };

    struct NodeIdLess
    {
        bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
        {
            return lhs.AsPointer() < rhs.AsPointer();
        }
    };

    static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
    {
        using namespace ImGui;
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiID id = window->GetID("##Splitter");
        ImRect bb;
        bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
        bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
        return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
    }
}

#endif //STUDIOSLAB_TEST2DEFS_H
