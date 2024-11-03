//
// Created by joao on 27/09/23.
//

#include "ImGuiModuleGLUT.h"

#include "3rdParty/imgui/backends/imgui_impl_glut.h"
#include "3rdParty/imgui/backends/imgui_impl_opengl3.h"

#include "Core/Backend/BackendManager.h"
#include "Core/Tools/Log.h"

// Don't touch
fix SHOULD_INSTALL_CALLBACKS = false;

namespace Slab::Graphics {

    static auto initializer = [](ImGuiContext *context) {
        ImGui::SetCurrentContext(context);

        // FIXME: Consider reworking this example to install our own GLUT funcs + forward calls ImGui_ImplGLUT_XXX ones, instead of using ImGui_ImplGLUT_InstallFuncs().
        // Install GLUT handlers (glutReshapeFunc(), glutMotionFunc(), glutPassiveMotionFunc(), glutMouseFunc(), glutKeyboardFunc() etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        ImGui_ImplGLUT_Init();
        ImGui_ImplOpenGL3_Init();
        if (SHOULD_INSTALL_CALLBACKS) ImGui_ImplGLUT_InstallFuncs();
    };

    ImGuiModuleGLUT::ImGuiModuleGLUT() : ImGuiModule(initializer) {
        try {
            auto glutBackend = DynamicPointerCast<GLUTBackend>(Core::BackendManager::GetBackend());
            glutBackend->addGLUTListener(Naked(*this));
        } catch (std::bad_cast& bad_cast) {
            throw StudioSlabRuntimeBackendInconsistency("while building ImGui module. Expected GLUT backend, "
                                                        "got something else");
        }

        Core::Log::Status() << "Initialized ImGui module (GLUT implementation)" << Core::Log::Flush;
    }

    ImGuiModuleGLUT::~ImGuiModuleGLUT()  {
        ImGui_ImplGLUT_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
    }

    void ImGuiModuleGLUT::beginRender() {
        ImGuiModule::beginRender();

        ImGui_ImplGLUT_NewFrame();
    }

    bool ImGuiModuleGLUT::keyboard(unsigned char key, int x, int y) {
        ImGui_ImplGLUT_KeyboardFunc(key, x, y);

        return ImGui::GetIO().WantCaptureKeyboard;
    }

    bool ImGuiModuleGLUT::keyboardUp(unsigned char key, int x, int y) {
        ImGui_ImplGLUT_KeyboardUpFunc(key, x, y);

        return ImGui::GetIO().WantCaptureKeyboard;
    }

    bool ImGuiModuleGLUT::keyboardSpecial(int key, int x, int y) {
        ImGui_ImplGLUT_SpecialFunc(key, x, y);

        return ImGui::GetIO().WantCaptureKeyboard;
    }

    bool ImGuiModuleGLUT::keyboardSpecialUp(int key, int x, int y) {
        ImGui_ImplGLUT_SpecialUpFunc(key, x, y);

        return ImGui::GetIO().WantCaptureKeyboard;
    }

    bool ImGuiModuleGLUT::mouseButton(int button, int dir, int x, int y) {
        ImGui_ImplGLUT_MouseFunc(button, dir, x, y);

        return ImGui::GetIO().WantCaptureMouse;
    }

    bool ImGuiModuleGLUT::mousePassiveMotion(int x, int y) {
        ImGui_ImplGLUT_MotionFunc(x, y);

        return ImGui::GetIO().WantCaptureMouse;
    }

    bool ImGuiModuleGLUT::mouseWheel(int wheel, int dir, int x, int y) {
        ImGui_ImplGLUT_MouseWheelFunc(wheel, dir, x, y);

        return ImGui::GetIO().WantCaptureMouse;
    }

    bool ImGuiModuleGLUT::mouseMotion(int x, int y) {
        ImGui_ImplGLUT_MotionFunc(x, y);

        return ImGui::GetIO().WantCaptureMouse;
    }

    bool ImGuiModuleGLUT::idle() { GLUTListener::idle(); return false; }

    bool ImGuiModuleGLUT::reshape(int w, int h) {
        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float) w, (float) h);

        return false;
    }

    void ImGuiModuleGLUT::endRender() {
        ImGuiModule::endRender();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
} // Core