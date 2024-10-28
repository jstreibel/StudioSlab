//
// Created by joao on 10/23/24.
//

#ifndef STUDIOSLAB_LAYOUT_H
#define STUDIOSLAB_LAYOUT_H

#include "3rdParty/ImGui.h"
#include "3rdParty/imgui/imgui_internal.h"

namespace ImGui {

    IMGUI_API void BeginHorizontal(const char *str_id, const ImVec2 &size = ImVec2(0, 0), float align = -1.0f);

    IMGUI_API void BeginHorizontal(const void *ptr_id, const ImVec2 &size = ImVec2(0, 0), float align = -1.0f);

    IMGUI_API void BeginHorizontal(int id, const ImVec2 &size = ImVec2(0, 0), float align = -1);

    IMGUI_API void EndHorizontal();

    IMGUI_API void BeginVertical(const char *str_id, const ImVec2 &size = ImVec2(0, 0), float align = -1.0f);

    IMGUI_API void BeginVertical(const void *ptr_id, const ImVec2 &size = ImVec2(0, 0), float align = -1.0f);

    IMGUI_API void BeginVertical(int id, const ImVec2 &size = ImVec2(0, 0), float align = -1);

    IMGUI_API void EndVertical();

    IMGUI_API void Spring(float weight = 1.0f, float spacing = -1.0f);

    IMGUI_API void SuspendLayout();

    IMGUI_API void ResumeLayout();
}

#endif //STUDIOSLAB_LAYOUT_H
