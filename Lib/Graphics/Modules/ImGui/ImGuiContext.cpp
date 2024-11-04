//
// Created by joao on 11/3/24.
//

#include "ImGuiContext.h"
#include "Utils/Exception.h"
#include "ImGuiModule.h"
#include "StudioSlab.h"
#include "3rdParty/imgui/imgui_internal.h"

namespace Slab::Graphics {

    SlabImGuiContext::SlabImGuiContext(ImGuiContext *context)
    : context(context) {    }

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

        for(auto &external_draw : external_draws) external_draw();

        external_draws.clear();
    }

    void SlabImGuiContext::Render() {
        auto &module = Slab::GetModule<ImGuiModule>("ImGui");

        module.RenderFrame();
    }

    ImGuiContext *SlabImGuiContext::getNativeContext() {
        return context;
    }

    void SlabImGuiContext::Bind() {
        ImGui::SetCurrentContext(context);
    }

    void SlabImGuiContext::AddExternalDraw(const ExternalDraw& external_draw) {
        external_draws.emplace_back(external_draw);
    }

    Real SlabImGuiContext::getFontSize() const {
        auto &module = Slab::GetModule<ImGuiModule>("ImGui");

        return module.getFontSize();
    }

} // Slab::Graphics