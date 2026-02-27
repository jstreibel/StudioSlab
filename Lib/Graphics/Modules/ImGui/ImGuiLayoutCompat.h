//
// Created by Codex on 2/27/26.
//

#ifndef STUDIOSLAB_IMGUILAYOUTCOMPAT_H
#define STUDIOSLAB_IMGUILAYOUTCOMPAT_H

#include "3rdParty/ImGui.h"

namespace Slab::Graphics {

    class FImGuiLayoutCompat {
    public:
        static void BeginHorizontal(const char* p_ID = nullptr,
                                    const ImVec2& p_Size = ImVec2(0.0f, 0.0f),
                                    float p_Align = -1.0f);

        static void BeginHorizontal(const void* p_ID,
                                    const ImVec2& p_Size = ImVec2(0.0f, 0.0f),
                                    float p_Align = -1.0f);

        static void EndHorizontal();

        static void BeginVertical(const char* p_ID = nullptr,
                                  const ImVec2& p_Size = ImVec2(0.0f, 0.0f),
                                  float p_Align = -1.0f);

        static void BeginVertical(const void* p_ID,
                                  const ImVec2& p_Size = ImVec2(0.0f, 0.0f),
                                  float p_Align = -1.0f);

        static void EndVertical();

        static void Spring(float p_Weight = 1.0f, float p_Spacing = -1.0f);
    };

} // namespace Slab::Graphics

#endif // STUDIOSLAB_IMGUILAYOUTCOMPAT_H
