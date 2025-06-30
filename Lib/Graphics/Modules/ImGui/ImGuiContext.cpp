//
// Created by joao on 11/3/24.
//

#include "ImGuiContext.h"

#include <utility>
#include <filesystem>

#include "Utils/Exception.h"

#include "3rdParty/imgui/imgui_internal.h"

#include "Core/Tools/Log.h"
#include "Core/Tools/Resources.h"

#include "ImGuiModule.h"
#include "StudioSlab.h"

#include "Graphics/SlabGraphics.h"
#include "Graphics/Window/WindowStyles.h"

#include "Slab-ImGui-Interop.h"

namespace Slab::Graphics {
    // Touch
    fix FONT_INDEX_FOR_IMGUI = 10; //6;
#define FONT_SIZE_PIXELS Slab::Graphics::WindowStyle::font_size

    void buildFonts()
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

        auto &log = Core::Log::Info() << "ImGui loading glyph ranges: ";
        int i = 0;
        for (auto &v: vRanges) {
            if (v == 0) break;
            log << std::hex << v << (++i % 2 ? "-" : " ");
        }
        log << std::dec << Core::Log::Flush;

        ImGuiIO &io = ImGui::GetIO();
        auto fontName = Core::Resources::GetIndexedFontFileName(FONT_INDEX_FOR_IMGUI);

        if (!std::filesystem::exists(fontName)) throw Exception(Str("Font ") + fontName + " does not exist.");

        ImFontConfig fontConfig;
        fontConfig.OversampleH = 4;
        fontConfig.OversampleV = 4;
        fontConfig.PixelSnapH = false;
        auto font = io.Fonts->AddFontFromFileTTF(fontName.c_str(), FONT_SIZE_PIXELS, &fontConfig, &vRanges[0]);

        io.FontDefault = font;

        io.Fonts->Build();

        Core::Log::Info() << "ImGui using font '" << Core::Resources::ExportedFonts[FONT_INDEX_FOR_IMGUI] << "'." << Core::Log::Flush;

        //ImGui::PushFont(font);
    }

    SlabImGuiContext::SlabImGuiContext(FOwnerSystemWindow system_window, FCallSet calls)
    : GUIContext(system_window), CallSet(std::move(calls)) {
        r_Context = ImGui::CreateContext();

        ImGui::SetCurrentContext(r_Context);

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        CallSet.Init(system_window->GetRawPlatformWindowPointer());

        buildFonts();

        // ImGui::GetStyle().ScaleAllSizes(1.25);
        ImGui::GetIO().FontGlobalScale = 1;

        Core::Log::Info() << "Created ImGui context." << Core::Log::Flush;
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
        ImGui::SetCurrentContext(r_Context);

        CallSet.NewFrame();
        ImGui::NewFrame();

        FlushDrawCalls();
    }

    void SlabImGuiContext::Render() const {
        ImGui::Render();

        CallSet.Draw();
    }


    void SlabImGuiContext::Bind() {
        ImGui::SetCurrentContext(r_Context);
    }

    DevFloat SlabImGuiContext::GetFontSize() const {
        (void)this; // get rid of annoying "this method can be made static" warning.

        return FONT_SIZE_PIXELS;
    }

    bool SlabImGuiContext::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl,  modKeys.Mod_Ctrl == Press);
        io.AddKeyEvent(ImGuiMod_Shift, modKeys.Mod_Shift == Press);
        io.AddKeyEvent(ImGuiMod_Alt,   modKeys.Mod_Alt == Press);
        io.AddKeyEvent(ImGuiMod_Super, modKeys.Mod_Super == Press);

        io.AddKeyEvent(SlabToImGuiTranslate(key), state == Press);

        return io.WantCaptureKeyboard;
    }

    bool SlabImGuiContext::NotifyCharacter(UInt codepoint) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddInputCharacter(codepoint);

        return io.WantTextInput;
    }

    void SlabImGuiContext::CursorEntered(bool entered) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddFocusEvent(entered);
    }

    bool SlabImGuiContext::NotifyMouseButton(EMouseButton button, EKeyState state, EModKeys modKeys) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl,  modKeys.Mod_Ctrl == Press);
        io.AddKeyEvent(ImGuiMod_Shift, modKeys.Mod_Shift == Press);
        io.AddKeyEvent(ImGuiMod_Alt,   modKeys.Mod_Alt == Press);
        io.AddKeyEvent(ImGuiMod_Super, modKeys.Mod_Super == Press);

        io.AddMouseButtonEvent(button, state==Press);

        return io.WantCaptureMouse;
    }

    bool SlabImGuiContext::NotifyMouseMotion(int x, int y, int dx, int dy) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddMousePosEvent((float)x, (float)y);

        return io.WantCaptureMouse;
    }

    bool SlabImGuiContext::NotifyMouseWheel(double dx, double dy) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddMouseWheelEvent((float)dx, (float)dy);

        return io.WantCaptureMouse;
    }

    bool SlabImGuiContext::NotifySystemWindowReshape(int w, int h) {
        ImGuiIO& io = ImGui::GetIO();

        io.DisplaySize = ImVec2((float)w, (float)h);

        return false;
    }

    bool SlabImGuiContext::NotifyRender() {
        NewFrame();
        Render();
        return true;
    }

    void AddItem(const int CurrentDepth, const MainMenuItem& Item) {
        fix& Location = Item.Location;
        fix MaxDepth = Location.size();

        if (ImGui::BeginMenu(Location[CurrentDepth].c_str())) {
            if (CurrentDepth < MaxDepth-1)
                AddItem(CurrentDepth+1, Item);
            else {
                auto action = Item.Action;
                for(const auto & [Label, Shortcut, Selected, Enabled] : Item.SubItems) {
                    if (ImGui::MenuItem(Label.c_str(), Shortcut.c_str(), Selected, Enabled))
                        action(Label);
                }
            }

            ImGui::EndMenu();
        }
    };

    void SlabImGuiContext::AddMainMenuItem(MainMenuItem item) {
        AddDrawCall([item](){
            if(ImGui::BeginMainMenuBar()) {
                AddItem(0, item);

                ImGui::EndMainMenuBar();
            }
        });
    }

    void *SlabImGuiContext::GetContextPointer() {
        return r_Context;
    }

} // Slab::Graphics