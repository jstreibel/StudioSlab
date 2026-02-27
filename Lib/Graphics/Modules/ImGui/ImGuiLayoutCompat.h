//
// Created by Codex on 2/27/26.
//

#ifndef STUDIOSLAB_IMGUILAYOUTCOMPAT_H
#define STUDIOSLAB_IMGUILAYOUTCOMPAT_H

#include "3rdParty/ImGui.h"

namespace Slab::Graphics {

    class FImGuiLayoutCompat {
    public:
        static constexpr bool bRequestedForkProvider =
#if defined(STUDIOSLAB_IMGUI_LAYOUT_USE_FORK) && (STUDIOSLAB_IMGUI_LAYOUT_USE_FORK == 0)
                false;
#else
                true;
#endif

        static constexpr bool bHasForkProvider =
#if defined(IMGUI_HAS_STACK_LAYOUT)
                true;
#else
                false;
#endif

        static constexpr bool bUsesForkProvider = bRequestedForkProvider && bHasForkProvider;

        static const char* GetProviderName();

        static void BeginHorizontal(const char* p_ID = nullptr,
                                    const ImVec2& p_Size = ImVec2(0.0f, 0.0f),
                                    float p_Align = -1.0f);

        static void BeginHorizontal(const void* p_ID,
                                    const ImVec2& p_Size = ImVec2(0.0f, 0.0f),
                                    float p_Align = -1.0f);

        static void BeginHorizontal(int p_ID,
                                    const ImVec2& p_Size = ImVec2(0.0f, 0.0f),
                                    float p_Align = -1.0f);

        static void EndHorizontal();

        static void BeginVertical(const char* p_ID = nullptr,
                                  const ImVec2& p_Size = ImVec2(0.0f, 0.0f),
                                  float p_Align = -1.0f);

        static void BeginVertical(const void* p_ID,
                                  const ImVec2& p_Size = ImVec2(0.0f, 0.0f),
                                  float p_Align = -1.0f);

        static void BeginVertical(int p_ID,
                                  const ImVec2& p_Size = ImVec2(0.0f, 0.0f),
                                  float p_Align = -1.0f);

        static void EndVertical();

        static void Spring(float p_Weight = 1.0f, float p_Spacing = -1.0f);
    };

} // namespace Slab::Graphics

#endif // STUDIOSLAB_IMGUILAYOUTCOMPAT_H
