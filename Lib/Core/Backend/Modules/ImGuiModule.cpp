//
// Created by joao on 22/09/23.
//

#include "Core/Graphics/OpenGL/OpenGL.h"
#include "Core/Graphics/OpenGL/Utils.h"

#include <filesystem>
#include <GL/freeglut.h>
#include "ImGuiModule.h"

#include "Utils/Resources.h"

#include "../BackendManager.h"

#include "Core/Tools/Log.h"

#include "3rdParty/imgui/imgui.h"
#include "backends/imgui_impl_glut.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "3rdParty/ImGuiColorsSetup.h"

fix FONT_INDEX_FOR_IMGUI = 10; //6;

namespace Core {

    void ImGuiModule::GeneralInitialization() {
        // Setup Dear ImGui context
        // Themes: https://github.com/ocornut/imgui/issues/707

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();
        //SetupImGuiColors_BlackAndWhite();

        {
            ImGui::GetStyle().WindowPadding = {20, 10};
            ImGui::GetStyle().FramePadding = ImVec2(12.0f, 6.0f);
            ImGui::GetStyle().CellPadding = {9, 5};
            ImGui::GetStyle().ItemSpacing = ImVec2(17.0f, 8.0f);
            ImGui::GetStyle().ItemInnerSpacing = {16, 8};
            ImGui::GetStyle().IndentSpacing = 30;
            ImGui::GetStyle().ScrollbarSize = 18;
            ImGui::GetStyle().GrabMinSize = 18;

            ImGui::GetStyle().WindowBorderSize = 1.0f;
            ImGui::GetStyle().FrameBorderSize = 0.0f;

            ImGui::GetStyle().WindowRounding = 4.0f;
            ImGui::GetStyle().ChildRounding = 5.0f;
            ImGui::GetStyle().FrameRounding = 5.0f;
            ImGui::GetStyle().ScrollbarRounding = 3.0f;
            ImGui::GetStyle().GrabRounding = 4.0f;
            ImGui::GetStyle().TabRounding = 5;
        }
    }

    ImGuiModule::ImGuiModule(BackendImplementation backendImpl) {

        GeneralInitialization();

        // Setup Platform/Renderer backends
        switch (backendImpl) {
            case Uninitialized:
            case Headless:
                throw Exception("ImGui module must be used with a graphic backend already initialized");
            case GLFW:
                InstallInGLFW();
                break;
            case GLUT:
                InstallInGLUT();
                break;
            case SFML:
                InstallInSFML();
                break;
            case VTK:
                break;
        }

        BuildFonts();

        ImGui::GetStyle().ScaleAllSizes(1.5);
        ImGui::GetIO().FontGlobalScale = 1;
    }

    void ImGuiModule::InstallInGLUT() {
        // FIXME: Consider reworking this example to install our own GLUT funcs + forward calls ImGui_ImplGLUT_XXX ones, instead of using ImGui_ImplGLUT_InstallFuncs().
        // Install GLUT handlers (glutReshapeFunc(), glutMotionFunc(), glutPassiveMotionFunc(), glutMouseFunc(), glutKeyboardFunc() etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        ImGui_ImplGLUT_Init();
        ImGui_ImplOpenGL3_Init();
        if (false) ImGui_ImplGLUT_InstallFuncs();
    }

    void ImGuiModule::InstallInGLFW() {
        throw Exception("ImGui GLFW module not implemented (yet)");
        // ImGui_ImplGlfw_InitForOpenGL();
    }

    void ImGuiModule::InstallInSFML() {
        throw Exception("ImGui SFML module not implemented (yet)");
    }

    void ImGuiModule::BuildFonts() {
        {
            static const ImWchar ranges[] =
                    {
                            0x0020, 0x007F, // Basic Latin
                            0x00B0, 0x00BF, // Superscript / subscript
                            0x0391, 0x03C9, // Greek
                            0x03D0, 0x03F6,
                            0x2070, 0x209F, // Superscript / subscript
                            0x21A6, 0x21A6 + 1,
                            ImWchar("ℑ"[0]), ImWchar("ℜ"[0]),
                            ImWchar("ℱ"[0]), ImWchar("𝒵"[0]),
                            0x2200, 0x22FF, // Mathematical operators
                            0x2A00, 0x2AFF, // Supplemental mathematical operators
                            0x1D400, 0x1D7FF, // Mathematical alphanumeric symbols
                            0,
                    };
            ImFontGlyphRangesBuilder glyphRangesBuilder;
            glyphRangesBuilder.AddRanges(ranges);
            for (ImWchar c: {ImWchar(0x1D62) /* subscript 'i'*/, ImWchar(0x21A6)}) glyphRangesBuilder.AddChar(c);
            static ImVector<ImWchar> vRanges;
            glyphRangesBuilder.BuildRanges(&vRanges);

            auto &log = Log::Info() << "ImGui loading glyph ranges: ";
            int i = 0;
            for (auto &v: vRanges) {
                if (v == 0) break;
                log << std::hex << v << (++i % 2 ? "-" : " ");
            }
            log << std::dec << Log::Flush;

            ImGuiIO &io = ImGui::GetIO();
            auto fontName = Resources::fontFileName(FONT_INDEX_FOR_IMGUI);

            if (!std::filesystem::exists(fontName)) throw Str("Font ") + fontName + " does not exist.";

            ImFontConfig fontConfig;
            auto font = io.Fonts->AddFontFromFileTTF(fontName.c_str(), 26.0f, &fontConfig, &vRanges[0]);

            io.FontDefault = font;

            Log::Info() << "ImGui using font '" << Resources::fonts[FONT_INDEX_FOR_IMGUI] << "'." << Log::Flush;

            //ImGui::PushFont(font);
        }
    }

    void ImGuiModule::beginRender() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();
        // if (me.showDemo) ImGui::ShowDemoWindow();
    }

    void ImGuiModule::endRender() {
        ImGui::Render();
        OpenGLUtils::checkGLErrors("after ImGui::Render()");
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        OpenGLUtils::checkGLErrors("after ImGui_ImplOpenGL3_RenderDrawData");
    }

    ImGuiModule::~ImGuiModule() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGLUT_Shutdown();
        ImGui::DestroyContext();
    }

    bool ImGuiModule::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        auto &mouse = Core::BackendManager::GetGUIBackend().getMouseState();

        if(Common::Contains(std::vector{Core::Key_NUM_LOCK,
                                        Core::Key_HOME,
                                        Core::Key_DELETE,
                                        Core::Key_LEFT_SHIFT,
                                        Core::Key_RIGHT_SHIFT,
                                        Core::Key_LEFT_CONTROL,
                                        Core::Key_RIGHT_CONTROL,
                                        Core::Key_LEFT_ALT,
                                        Core::Key_RIGHT_ALT,
                                        Core::Key_LEFT_SUPER,
                                        Core::Key_RIGHT_SUPER}, key))
            ImGui_ImplGLUT_SpecialFunc(key, mouse.x, mouse.y);
        else
            ImGui_ImplGLUT_KeyboardFunc(key, mouse.x, mouse.y);

        if(ImGui::GetIO().WantCaptureKeyboard) return true;

        return GUIEventListener::notifyKeyboard(key, state, modKeys);
    }

    bool ImGuiModule::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        auto &mouse = Core::BackendManager::GetGUIBackend().getMouseState();

        int GLUTstate = state==Press ? GLUT_DOWN : GLUT_UP;
        int GLUTbutton = button==MouseButton_LEFT ? GLUT_LEFT_BUTTON
                       : button==MouseButton_MIDDLE ? GLUT_MIDDLE_BUTTON
                       : button==MouseButton_RIGHT ? GLUT_RIGHT_BUTTON
                       : -1;

        ImGui_ImplGLUT_MouseFunc(GLUTbutton, GLUTstate, mouse.x, mouse.y);

        if(ImGui::GetIO().WantCaptureMouse)
            return true;

        return GUIEventListener::notifyMouseButton(button, state, keys);
    }

    bool ImGuiModule::notifyMouseMotion(int x, int y) {
        ImGui_ImplGLUT_MotionFunc(x, y);
        return ImGui::GetIO().WantCaptureMouse;
    }

    bool ImGuiModule::notifyMouseWheel(double dx, double dy) {
        auto &mouse = Core::BackendManager::GetGUIBackend().getMouseState();
        ImGui_ImplGLUT_MouseWheelFunc(0, dx, mouse.x, mouse.y);

        if(ImGui::GetIO().WantCaptureMouse) return true;

        return GUIEventListener::notifyMouseWheel(dx, dy);
    }

    bool ImGuiModule::notifyFilesDropped(StrVector paths) {
        return GUIEventListener::notifyFilesDropped(paths);
    }

    bool ImGuiModule::notifyScreenReshape(int w, int h) {
        {
            ImGuiIO &io = ImGui::GetIO();
            io.DisplaySize = ImVec2((float) w, (float) h);
        }

        return GUIEventListener::notifyScreenReshape(w, h);
    }

    bool ImGuiModule::notifyRender() {
        return GUIEventListener::notifyRender();
    }


} // Core