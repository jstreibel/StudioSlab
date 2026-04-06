//
// Created by Codex on 2/27/26.
//

#include "Graphics/Modules/ImGui/ImGuiFrameCompat.h"

#include <imgui_internal.h>

namespace Slab::Graphics {

    bool FImGuiFrameCompat::CanIssueDrawCommands()
    {
        if (GImGui == nullptr) {
            return false;
        }

        return GImGui->WithinFrameScope;
    }

} // namespace Slab::Graphics
