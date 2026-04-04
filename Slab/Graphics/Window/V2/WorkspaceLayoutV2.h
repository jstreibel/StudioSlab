#ifndef STUDIOSLAB_WORKSPACE_LAYOUT_V2_H
#define STUDIOSLAB_WORKSPACE_LAYOUT_V2_H

#include "Utils/Arrays.h"
#include "imgui.h"

#include <map>

namespace Slab::Graphics::Windowing::V2 {

    enum class EDockSplitDirectionV2 : unsigned char {
        Left = 0,
        Right,
        Up,
        Down
    };

    struct FWorkspaceDefinitionV2 {
        Str WorkspaceId;
        Str DisplayName;
        bool bUsesSlabWindows = false;
    };

    struct FDockNodeSplitV2 {
        Str SourceNodeId = "main";
        Str NewNodeId;
        Str RemainingNodeId = "main";
        EDockSplitDirectionV2 Direction = EDockSplitDirectionV2::Left;
        float Ratio = 0.5f;
    };

    struct FDockWindowPlacementV2 {
        Str WindowTitle;
        Str NodeId = "main";
        bool bSelectByDefault = false;
    };

    struct FWorkspaceDockLayoutV2 {
        Vector<FDockNodeSplitV2> Splits;
        Vector<FDockWindowPlacementV2> Placements;
    };

    auto ApplyDockLayoutToImGui(ImGuiID dockspaceId,
                                const ImVec2 &dockSize,
                                const FWorkspaceDockLayoutV2 &layout,
                                std::map<Str, ImGuiID> *outDockNodes = nullptr) -> bool;

} // namespace Slab::Graphics::Windowing::V2

#endif // STUDIOSLAB_WORKSPACE_LAYOUT_V2_H
