//
// Created by joao on 11/3/24.
//

#include "ImGuiContext.h"
#include "Utils/Exception.h"
#include "ImGuiModule.h"
#include "StudioSlab.h"
#include "3rdParty/imgui/imgui_internal.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame(); // Necessario!!
        ImGui::NewFrame();

        for(auto &external_draw : external_draws) external_draw();

        external_draws.clear();
    }


    void SlabImGuiContext::Bind() {
        ImGui::SetCurrentContext(context);
    }

    void SlabImGuiContext::AddDrawCall(const DrawCall& draw_call) {
        external_draws.emplace_back(draw_call);
    }

    Real SlabImGuiContext::getFontSize() const {
        (void)this; // get rid of annoying "this method can be made static" warning.

        auto &module = Slab::GetModule<ImGuiModule>("ImGui");

        return module.getFontSize();
    }

    bool SlabImGuiContext::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        // io.AddKeyEvent(ImGuiMod_Ctrl,  (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS));
        // io.AddKeyEvent(ImGuiMod_Shift, (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)   == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)   == GLFW_PRESS));
        // io.AddKeyEvent(ImGuiMod_Alt,   (glfwGetKey(window, GLFW_KEY_LEFT_ALT)     == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT)     == GLFW_PRESS));
        // io.AddKeyEvent(ImGuiMod_Super, (glfwGetKey(window, GLFW_KEY_LEFT_SUPER)   == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER)   == GLFW_PRESS));

        return SystemWindowEventListener::notifyKeyboard(key, state, modKeys);
    }

    bool SlabImGuiContext::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        return SystemWindowEventListener::notifyMouseButton(button, state, keys);
    }

    bool SlabImGuiContext::notifyMouseMotion(int x, int y, int dx, int dy) {
        return SystemWindowEventListener::notifyMouseMotion(x, y, dx, dy);
    }

    bool SlabImGuiContext::notifyMouseWheel(double dx, double dy) {
        return SystemWindowEventListener::notifyMouseWheel(dx, dy);
    }

    bool SlabImGuiContext::notifyFilesDropped(StrVector paths) {
        return SystemWindowEventListener::notifyFilesDropped(paths);
    }

    bool SlabImGuiContext::notifySystemWindowReshape(int w, int h) {
        return SystemWindowEventListener::notifySystemWindowReshape(w, h);
    }

    bool SlabImGuiContext::notifyRender() {
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        return true;
    }

} // Slab::Graphics