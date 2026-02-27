//
// Created by Codex on 2/27/26.
//

#include "Graphics/Modules/ImGui/ImGuiLayoutCompat.h"

#include <vector>

#if ((!defined(STUDIOSLAB_IMGUI_LAYOUT_USE_FORK) || (STUDIOSLAB_IMGUI_LAYOUT_USE_FORK != 0)) && defined(IMGUI_HAS_STACK_LAYOUT))
#define STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK 1
#else
#define STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK 0
#endif

namespace {

    enum class ECompatLayoutType
    {
        Horizontal,
        Vertical
    };

    struct FCompatLayoutState
    {
        ECompatLayoutType Type = ECompatLayoutType::Horizontal;
        bool bSegmentOpen = false;
        ImGuiContext* Context = nullptr;
    };

    static std::vector<FCompatLayoutState> GCompatLayoutStack;

    static float ResolveSpacing(ECompatLayoutType p_Type, float p_Spacing)
    {
        if (p_Spacing >= 0.0f) {
            return p_Spacing;
        }

        const ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;
        return p_Type == ECompatLayoutType::Horizontal ? itemSpacing.x : itemSpacing.y;
    }

    static FCompatLayoutState* GetCurrentCompatState()
    {
        if (GCompatLayoutStack.empty()) {
            return nullptr;
        }

        FCompatLayoutState& state = GCompatLayoutStack.back();
        if (state.Context != ImGui::GetCurrentContext()) {
            GCompatLayoutStack.clear();
            return nullptr;
        }

        return &state;
    }

    static void BeginCompatLayout(ECompatLayoutType p_Type)
    {
        ImGuiContext* context = ImGui::GetCurrentContext();
        IM_ASSERT(context != nullptr);

        ImGui::BeginGroup(); // layout frame
        ImGui::BeginGroup(); // first segment

        FCompatLayoutState state;
        state.Type = p_Type;
        state.bSegmentOpen = true;
        state.Context = context;
        GCompatLayoutStack.push_back(state);
    }

    static void EndCompatLayout(ECompatLayoutType p_Type)
    {
        FCompatLayoutState* state = GetCurrentCompatState();
        IM_ASSERT(state != nullptr);
        IM_ASSERT(state->Type == p_Type);

        if (state == nullptr) {
            return;
        }

        if (state->bSegmentOpen) {
            ImGui::EndGroup();
        }
        ImGui::EndGroup();

        GCompatLayoutStack.pop_back();
    }

    static void AddCompatSpring(float p_Weight, float p_Spacing)
    {
        (void)p_Weight; // Fallback implementation ignores spring weight.

        FCompatLayoutState* state = GetCurrentCompatState();
        IM_ASSERT(state != nullptr);
        if (state == nullptr) {
            return;
        }

        if (state->bSegmentOpen) {
            ImGui::EndGroup();
            state->bSegmentOpen = false;
        }

        const float spacing = ResolveSpacing(state->Type, p_Spacing);

        if (state->Type == ECompatLayoutType::Horizontal) {
            ImGui::SameLine(0.0f, spacing);
        } else if (spacing > 0.0f) {
            // Use explicit spacing between vertical segments.
            ImGui::Dummy(ImVec2(0.0f, spacing));
        }

        ImGui::BeginGroup();
        state->bSegmentOpen = true;
    }

} // namespace

namespace Slab::Graphics {

    const char* FImGuiLayoutCompat::GetProviderName()
    {
#if STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK
        return "fork-stack-layout";
#else
        return "compat-fallback";
#endif
    }

    void FImGuiLayoutCompat::BeginHorizontal(const char* p_ID, const ImVec2& p_Size, float p_Align)
    {
#if STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK
        ImGui::BeginHorizontal(p_ID, p_Size, p_Align);
#else
        (void)p_ID;
        (void)p_Size;
        (void)p_Align;
        BeginCompatLayout(ECompatLayoutType::Horizontal);
#endif
    }

    void FImGuiLayoutCompat::BeginHorizontal(const void* p_ID, const ImVec2& p_Size, float p_Align)
    {
#if STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK
        ImGui::BeginHorizontal(p_ID, p_Size, p_Align);
#else
        (void)p_ID;
        (void)p_Size;
        (void)p_Align;
        BeginCompatLayout(ECompatLayoutType::Horizontal);
#endif
    }

    void FImGuiLayoutCompat::BeginHorizontal(int p_ID, const ImVec2& p_Size, float p_Align)
    {
#if STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK
        ImGui::BeginHorizontal(p_ID, p_Size, p_Align);
#else
        (void)p_ID;
        (void)p_Size;
        (void)p_Align;
        BeginCompatLayout(ECompatLayoutType::Horizontal);
#endif
    }

    void FImGuiLayoutCompat::EndHorizontal()
    {
#if STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK
        ImGui::EndHorizontal();
#else
        EndCompatLayout(ECompatLayoutType::Horizontal);
#endif
    }

    void FImGuiLayoutCompat::BeginVertical(const char* p_ID, const ImVec2& p_Size, float p_Align)
    {
#if STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK
        ImGui::BeginVertical(p_ID, p_Size, p_Align);
#else
        (void)p_ID;
        (void)p_Size;
        (void)p_Align;
        BeginCompatLayout(ECompatLayoutType::Vertical);
#endif
    }

    void FImGuiLayoutCompat::BeginVertical(const void* p_ID, const ImVec2& p_Size, float p_Align)
    {
#if STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK
        ImGui::BeginVertical(p_ID, p_Size, p_Align);
#else
        (void)p_ID;
        (void)p_Size;
        (void)p_Align;
        BeginCompatLayout(ECompatLayoutType::Vertical);
#endif
    }

    void FImGuiLayoutCompat::BeginVertical(int p_ID, const ImVec2& p_Size, float p_Align)
    {
#if STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK
        ImGui::BeginVertical(p_ID, p_Size, p_Align);
#else
        (void)p_ID;
        (void)p_Size;
        (void)p_Align;
        BeginCompatLayout(ECompatLayoutType::Vertical);
#endif
    }

    void FImGuiLayoutCompat::EndVertical()
    {
#if STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK
        ImGui::EndVertical();
#else
        EndCompatLayout(ECompatLayoutType::Vertical);
#endif
    }

    void FImGuiLayoutCompat::Spring(float p_Weight, float p_Spacing)
    {
#if STUDIOSLAB_IMGUI_LAYOUT_COMPILE_FORK
        ImGui::Spring(p_Weight, p_Spacing);
#else
        AddCompatSpring(p_Weight, p_Spacing);
#endif
    }

} // namespace Slab::Graphics
