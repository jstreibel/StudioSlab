//
// Created by joao on 30/05/23.
//

#ifndef STUDIOSLAB_UTILS_H
#define STUDIOSLAB_UTILS_H

#include "imgui/imgui.h"

void SetupImGuiColors_BlackAndWhite () {
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImGui::GetStyle().FramePadding = ImVec2(4.0f,2.0f);
    ImGui::GetStyle().ItemSpacing = ImVec2(8.0f,2.0f);
    ImGui::GetStyle().WindowRounding = 2.0f;
    ImGui::GetStyle().ChildRounding = 2.0f;
    ImGui::GetStyle().FrameRounding = 0.0f;
    ImGui::GetStyle().ScrollbarRounding = 0.0f;
    ImGui::GetStyle().GrabRounding = 1.0f;
    ImGui::GetStyle().WindowBorderSize = 1.0f;
    ImGui::GetStyle().FrameBorderSize = 1.0f;
    colors[ImGuiCol_Text]                   = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.44f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.64f, 0.65f, 0.66f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.64f, 0.65f, 0.66f, 0.40f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.71f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.63f, 0.63f, 0.63f, 0.78f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.43f, 0.44f, 0.46f, 0.78f);
    colors[ImGuiCol_Button]                 = ImVec4(0.61f, 0.61f, 0.62f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.57f, 0.57f, 0.57f, 0.52f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.61f, 0.63f, 0.64f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.64f, 0.64f, 0.65f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.58f, 0.58f, 0.59f, 0.55f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.52f, 0.52f, 0.52f, 0.55f);
    colors[ImGuiCol_Separator]              = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.17f, 0.17f, 0.17f, 0.89f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.17f, 0.17f, 0.17f, 0.89f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.39f, 0.39f, 0.40f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.39f, 0.39f, 0.40f, 0.67f);
    //colors[ImGuiCol_CloseButton]            = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
    //colors[ImGuiCol_CloseButtonHovered]     = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    //colors[ImGuiCol_CloseButtonActive]      = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.71f, 0.72f, 0.73f, 0.57f);
    //colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.16f, 0.16f, 0.17f, 0.95f);
}

#endif //STUDIOSLAB_UTILS_H
