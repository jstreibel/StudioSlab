//
// Created by joao on 11/3/24.
//

#include "ImGuiContext.h"
#include "Utils/Exception.h"
#include "ImGuiModule.h"
#include "StudioSlab.h"

namespace Slab::Graphics {

    SlabImGuiContext::SlabImGuiContext(ImGuiContext *context) : context(context) {

    }

    // SlabImGuiContext::SlabImGuiContext() {
    //     context = ImGui::CreateContext();
    //     ImGui::SetCurrentContext(context);
    // }

    //SlabImGuiContext::~SlabImGuiContext() {
    //    // ImGui::SetCurrentContext(context);
    //    ImGui::DestroyContext(context);
    //}

    void SlabImGuiContext::NewFrame() {
        ImGui::SetCurrentContext(context);

        auto &imgui_module = Slab::GetModule<ImGuiModule>("ImGui");

        imgui_module.NewFrame();
    }

    void SlabImGuiContext::Render() {
        auto &module = Slab::GetModule<ImGuiModule>("ImGui");
        module.RenderFrame();
    }

} // Slab::Graphics