//
// Created by joao on 22/09/23.
//

#include "Graphics/OpenGL/Utils.h"

#include <filesystem>

#include "ImGuiModule.h"

#include "Utils/Resources.h"

#include "Core/Backend/BackendManager.h"
#include "Core/Tools/Log.h"

#include "ImGuiColorAndStyles.h"

#include "ImGuiModuleGLUT.h"
#include "ImGuiModuleGLFW.h"
#include "backends/imgui_impl_opengl3.h"

// Touch
fix FONT_INDEX_FOR_IMGUI = 10; //6;

namespace Core {

    // ** THEMES everybody **
    // https://github.com/ocornut/imgui/issues/707

    void ImGuiModule::generalInitialization() {
        // Setup Dear ImGui context

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        // Setup Dear ImGui style
        // SetColorThemeNativeLight();
        // SetColorThemeNativeDark();
        // SetColorThemeDarkRed();
        SetStyleDark();
        //SetStyleLight();
        SetStyleStudioSlab();
    }

    ImGuiModule::ImGuiModule(BackendImplementation backendImpl) {
        system = backendImpl;

        generalInitialization();
    }

    ImGuiModule::~ImGuiModule() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiModule::buildFonts()
    {
        static const ImWchar ranges[] =
                {
                        0x0020, 0x007F, // Basic Latin
                        0x00B0, 0x00BF, // Superscript / subscript
                        0x0391, 0x03C9, // Greek
                        0x03D0, 0x03F6,
                        0x2000, 0x2311, // Math stuff
                        /*0x2070, 0x209F, // Superscript / subscript
                        0x21A6, 0x21A6 + 1,
                        ImWchar("ℑ"[0]), ImWchar("ℜ"[0]),
                        ImWchar("ℱ"[0]), ImWchar("𝒵"[0]),
                        ImWchar("𝔸"[0]), ImWchar("ℤ"[0]),
                        0x2200, 0x22FF, // Mathematical operators
                        0x2A00, 0x2AFF, // Supplemental mathematical operators */
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

        if (!std::filesystem::exists(fontName)) throw Exception(Str("Font ") + fontName + " does not exist.");

        ImFontConfig fontConfig;
        auto font = io.Fonts->AddFontFromFileTTF(fontName.c_str(), 26.0f, &fontConfig, &vRanges[0]);

        io.FontDefault = font;

        Log::Info() << "ImGui using font '" << Resources::fonts[FONT_INDEX_FOR_IMGUI] << "'." << Log::Flush;

        //ImGui::PushFont(font);
    }

    void ImGuiModule::finishInitialization() {
        buildFonts();

        ImGui::GetStyle().ScaleAllSizes(1.5);
        ImGui::GetIO().FontGlobalScale = 1;
    }

    ImGuiModule* ImGuiModule::BuildModule(BackendImplementation backendImplementation) {
        switch (backendImplementation) {
            case Uninitialized:
            case Headless:
                throw NotImplementedException("Headless ImGui module");
            case GLFW:
                return new ImGuiModuleGLFW();
            case SFML:
                throw NotImplementedException("SFML ImGui module");
            case VTK:
                throw NotImplementedException("VTK ImGui module");
            case GLUT:
                return new ImGuiModuleGLUT();
        }

        NOT_IMPLEMENTED
    }

    void ImGuiModule::beginRender() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        if (showDemos)
            ImGui::ShowDemoWindow();
    }

    void ImGuiModule::endRender() {
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }


} // Core