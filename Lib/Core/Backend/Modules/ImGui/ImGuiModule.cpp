//
// Created by joao on 22/09/23.
//

#include "3rdParty/ImGui.h"

#ifndef IMGUI_USE_WCHAR32
#pragma error
#endif

// #include "Graphics/OpenGL/Utils.h"

#include <filesystem>

#include "ImGuiModule.h"

#include "Core/Backend/BackendManager.h"
#include "Core/Tools/Resources.h"
#include "Core/Tools/Log.h"

#include "ImGuiColorAndStyles.h"

#include "ImGuiModuleGLUT.h"
#include "ImGuiModuleGLFW.h"

// Touch
fix FONT_INDEX_FOR_IMGUI = 10; //6;
fix FONT_SIZE_PIXELS = 20.0f;

namespace Slab::Core {

    // ** THEMES everybody **
    // https://github.com/ocornut/imgui/issues/707

    Str currentTheme = "Dark";
    std::map<Str, void(*)()> colorThemes = {
            {"Native light", Slab::Core::SetColorThemeNativeLight},
            {"Native dark", Slab::Core::SetColorThemeNativeDark},
            {"Dark red", Slab::Core::SetColorThemeDarkRed},
            {"Dark", Slab::Core::SetStyleDark},
            {"Light", Slab::Core::SetStyleLight},
            {"StudioSlab", Slab::Core::SetStyleStudioSlab}
    };

    void ImGuiModule::generalInitialization() {
        // Setup Dear ImGui context

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        // Setup Dear ImGui style
        SetStyleStudioSlab();   // For sizes
        colorThemes[currentTheme]();
    }

    ImGuiModule::ImGuiModule(BackendImplementation backendImpl) : GraphicsModule("ImGui") {
        system = backendImpl;

        generalInitialization();
    }

    ImGuiModule::~ImGuiModule() {
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
        for (ImWchar c: {ImWchar(0x1D62) /* subscript 'i'*/,
                         ImWchar(0x21A6),
                         /*ImWchar("⟨"[0]),
                         ImWchar("⟩"[0])*/}
                         ) glyphRangesBuilder.AddChar(c);
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
        auto font = io.Fonts->AddFontFromFileTTF(fontName.c_str(), FONT_SIZE_PIXELS, &fontConfig, &vRanges[0]);

        io.FontDefault = font;

        Log::Info() << "ImGui using font '" << Resources::fonts[FONT_INDEX_FOR_IMGUI] << "'." << Log::Flush;

        //ImGui::PushFont(font);
    }

    void ImGuiModule::finishInitialization() {
        buildFonts();

        ImGui::GetStyle().ScaleAllSizes(1.25);
        ImGui::GetIO().FontGlobalScale = 1;
    }

    ImGuiModule* ImGuiModule::BuildModule(BackendImplementation backendImplementation) {
        switch (backendImplementation) {
            case Uninitialized:
                throw NotImplementedException("ImGui must be built with a GUI backend already initialized.");
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
        ImGui::NewFrame();

        if (showDemos)
            ImGui::ShowDemoWindow();

        ImGui::BeginMainMenuBar();
        {
            if(ImGui::BeginMenu("Menu")) {
                if(ImGui::MenuItem("Quit", "Alt+F4")) Core::BackendManager::GetBackend().terminate();

                ImGui::EndMenu();
            }

            if(ImGui::BeginMenu("Style")) {
                if(ImGui::BeginMenu("GUI")) {
                    for(const auto& theme : colorThemes) {
                        if (ImGui::MenuItem(theme.first.c_str(), nullptr,
                                            theme.first==currentTheme)) {
                            theme.second();
                            currentTheme = theme.first;
                        }
                    }
                    ImGui::MenuItem("---", nullptr, false, false);
                    ImGui::MenuItem("Show ImGui demos", nullptr, &showDemos);

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void ImGuiModule::endRender() {
        ImGui::Render();
    }


} // Core