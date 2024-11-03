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

        auto &module = Slab::GetModule("ImGui");
        auto &imgui_module = dynamic_cast<ImGuiModule&>(module);

        imgui_module.newFrame();
    }

    void SlabImGuiContext::EndFrame(bool should_render) {
        if(ImGui::GetCurrentContext() != context) throw Exception("Calling GUI EndFrame on non-bound context.");

        auto &module = Slab::GetModule("ImGui");
        auto &imgui_module = dynamic_cast<ImGuiModule&>(module);

        NOT_IMPLEMENTED

        // if(!should_render){
        //     imgui_module.EndFrame();
//
        //     return;
        // }
//
        // imgui_module.Render();
    }

} // Slab::Graphics