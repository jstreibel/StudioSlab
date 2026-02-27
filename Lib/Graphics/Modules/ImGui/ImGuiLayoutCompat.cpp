//
// Created by Codex on 2/27/26.
//

#include "Graphics/Modules/ImGui/ImGuiLayoutCompat.h"

#if !defined(IMGUI_HAS_STACK_LAYOUT)
#error "FImGuiLayoutCompat currently requires IMGUI_HAS_STACK_LAYOUT from the fork provider."
#endif

namespace Slab::Graphics {

    void FImGuiLayoutCompat::BeginHorizontal(const char* p_ID, const ImVec2& p_Size, float p_Align)
    {
        ImGui::BeginHorizontal(p_ID, p_Size, p_Align);
    }

    void FImGuiLayoutCompat::BeginHorizontal(const void* p_ID, const ImVec2& p_Size, float p_Align)
    {
        ImGui::BeginHorizontal(p_ID, p_Size, p_Align);
    }

    void FImGuiLayoutCompat::EndHorizontal()
    {
        ImGui::EndHorizontal();
    }

    void FImGuiLayoutCompat::BeginVertical(const char* p_ID, const ImVec2& p_Size, float p_Align)
    {
        ImGui::BeginVertical(p_ID, p_Size, p_Align);
    }

    void FImGuiLayoutCompat::BeginVertical(const void* p_ID, const ImVec2& p_Size, float p_Align)
    {
        ImGui::BeginVertical(p_ID, p_Size, p_Align);
    }

    void FImGuiLayoutCompat::EndVertical()
    {
        ImGui::EndVertical();
    }

    void FImGuiLayoutCompat::Spring(float p_Weight, float p_Spacing)
    {
        ImGui::Spring(p_Weight, p_Spacing);
    }

} // namespace Slab::Graphics
