//
// Created by joao on 22/09/23.
//

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

#include "Core/Backend/GLFW/GLFWBackend.h"

// Don't touch
fix DO_NOT_INSTALL_CALLBACKS = false;

// Touch
fix FONT_INDEX_FOR_IMGUI = 10; //6;

namespace Core {

    void ImGuiModule::GeneralInitialization() {
        // Setup Dear ImGui context

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();
        // SetupImGuiColors_BlackAndWhite();

        {
            // ** THEMES everybody **
            // https://github.com/ocornut/imgui/issues/707
            if(true){
                ImGuiStyle& style = ImGui::GetStyle();
                style.Colors[ImGuiCol_Text]                  = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
                style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
                style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
                style.Colors[ImGuiCol_ChildBg]               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
                style.Colors[ImGuiCol_PopupBg]               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
                style.Colors[ImGuiCol_Border]                = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
                style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                style.Colors[ImGuiCol_FrameBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
                style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
                style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
                style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
                style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
                style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
                style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
                style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
                style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
                style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
                style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
                style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
                style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
                style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
                style.Colors[ImGuiCol_Button]                = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
                style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
                style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
                style.Colors[ImGuiCol_Header]                = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
                style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
                style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
                style.Colors[ImGuiCol_Separator]             = style.Colors[ImGuiCol_Border];
                style.Colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
                style.Colors[ImGuiCol_SeparatorActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
                style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
                style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
                style.Colors[ImGuiCol_Tab]                   = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
                style.Colors[ImGuiCol_TabHovered]            = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
                style.Colors[ImGuiCol_TabActive]             = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
                style.Colors[ImGuiCol_TabUnfocused]          = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
                style.Colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
                // style.Colors[ImGuiCol_DockingPreview]        = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
                // style.Colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
                style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
                style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
                style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
                style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
                style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
                style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
                style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
                style.Colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
                style.Colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
                style.GrabRounding                           = style.FrameRounding = 2.3f;

            }
            else {
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

        system = backendImpl;
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
        auto &window = dynamic_cast<GLFWBackend*>(&Core::BackendManager::GetGUIBackend())->getGLFWWindow();

        ImGui_ImplGlfw_InitForOpenGL(&window, DO_NOT_INSTALL_CALLBACKS);
    }

    void ImGuiModule::InstallInSFML() {
        throw Exception("ImGui SFML module not implemented (yet)");
    }

    void ImGuiModule::BuildFonts()
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

    void ImGuiModule::beginRender() {
        ImGui_ImplOpenGL3_NewFrame();

        switch (system) {
            case Uninitialized:
            case Headless:
            case SFML:
            case VTK:
                NOT_IMPLEMENTED
            case GLFW:
                ImGui_ImplGlfw_NewFrame();
                break;
            case GLUT:
                ImGui_ImplGLUT_NewFrame();
                break;
        }

        ImGui::NewFrame();
    }

    void ImGuiModule::endRender() {
        ImGui::Render();
        OpenGLUtils::checkGLErrors("after ImGui::Render()");

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        OpenGLUtils::checkGLErrors("after ImGui_ImplOpenGL3_RenderDrawData");
    }

    ImGuiModule::~ImGuiModule() {
        ImGui_ImplOpenGL3_Shutdown();

        switch (system) {
            case Uninitialized:
            case Headless:
            case SFML:
            case VTK:
                break;
            case GLFW:
                ImGui_ImplGlfw_Shutdown();
                break;
            case GLUT:
                ImGui_ImplGLUT_Shutdown();
                break;
        }

        ImGui::DestroyContext();
    }

    bool ImGuiModule::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        auto &mouse = Core::BackendManager::GetGUIBackend().getMouseState();

        if(system == GLUT) {
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
        } else NOT_IMPLEMENTED


        if(ImGui::GetIO().WantCaptureKeyboard) return true;

        if(state == Press) {
            if (key == Key_d) {
                showDemos = !showDemos;
                return true;
            }
        }

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
        if (showDemos) ImGui::ShowDemoWindow();

        return GUIEventListener::notifyRender();
    }


} // Core