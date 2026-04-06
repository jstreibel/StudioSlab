//
// Created by Codex on 2/27/26.
//

#include "Graphics/Modules/ImGui/ImGuiSplitterCompat.h"

#include <imgui_internal.h>

namespace Slab::Graphics {

    bool FImGuiSplitterCompat::Splitter(bool p_SplitVertically,
                                        float p_Thickness,
                                        float* p_Size1,
                                        float* p_Size2,
                                        float p_MinSize1,
                                        float p_MinSize2,
                                        float p_SplitterLongAxisSize)
    {
        IM_ASSERT(p_Size1 != nullptr);
        IM_ASSERT(p_Size2 != nullptr);

        using namespace ImGui;

        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiID id = window->GetID("##Splitter");

        ImRect bb;
        bb.Min = window->DC.CursorPos + (p_SplitVertically ? ImVec2(*p_Size1, 0.0f) : ImVec2(0.0f, *p_Size1));
        bb.Max = bb.Min + CalcItemSize(p_SplitVertically ? ImVec2(p_Thickness, p_SplitterLongAxisSize)
                                                          : ImVec2(p_SplitterLongAxisSize, p_Thickness),
                                        0.0f,
                                        0.0f);

        return SplitterBehavior(bb,
                                id,
                                p_SplitVertically ? ImGuiAxis_X : ImGuiAxis_Y,
                                p_Size1,
                                p_Size2,
                                p_MinSize1,
                                p_MinSize2,
                                0.0f);
    }

} // namespace Slab::Graphics
