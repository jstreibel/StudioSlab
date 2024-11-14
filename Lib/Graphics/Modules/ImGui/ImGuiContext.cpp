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
        auto fontName = Core::Resources::fontFileName(FONT_INDEX_FOR_IMGUI);

        if (!std::filesystem::exists(fontName)) throw Exception(Str("Font ") + fontName + " does not exist.");

        ImFontConfig fontConfig;
        fontConfig.OversampleH = 4;
        fontConfig.OversampleV = 4;
        fontConfig.PixelSnapH = false;
        auto font = io.Fonts->AddFontFromFileTTF(fontName.c_str(), FONT_SIZE_PIXELS, &fontConfig, &vRanges[0]);

        io.FontDefault = font;

        io.Fonts->Build();

        Core::Log::Info() << "ImGui using font '" << Core::Resources::fonts[FONT_INDEX_FOR_IMGUI] << "'." << Core::Log::Flush;

        //ImGui::PushFont(font);
    }

    SlabImGuiContext::SlabImGuiContext(RawSystemWindowPointer raw_syswin_ptr, CallSet calls)
    : call_set(std::move(calls)) {
        context = ImGui::CreateContext();

        ImGui::SetCurrentContext(context);

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        call_set.Init(raw_syswin_ptr);

        buildFonts();

        // ImGui::GetStyle().ScaleAllSizes(1.25);
        ImGui::GetIO().FontGlobalScale = 1;
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

        call_set.NewFrame();
        ImGui::NewFrame();

        FlushDrawCalls();
    }

    void SlabImGuiContext::Render() const {
        ImGui::Render();

        call_set.Draw();
    }


    void SlabImGuiContext::Bind() {
        ImGui::SetCurrentContext(context);
    }

    Real SlabImGuiContext::getFontSize() const {
        (void)this; // get rid of annoying "this method can be made static" warning.

        return FONT_SIZE_PIXELS;
    }

    bool SlabImGuiContext::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl,  modKeys.Mod_Ctrl == Press);
        io.AddKeyEvent(ImGuiMod_Shift, modKeys.Mod_Shift == Press);
        io.AddKeyEvent(ImGuiMod_Alt,   modKeys.Mod_Alt == Press);
        io.AddKeyEvent(ImGuiMod_Super, modKeys.Mod_Super == Press);

        io.AddKeyEvent(SlabToImGuiTranslate(key), state == Press);

        return io.WantCaptureKeyboard;
    }

    bool SlabImGuiContext::notifyCharacter(UInt codepoint) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddInputCharacter(codepoint);

        return io.WantTextInput;
    }

    void SlabImGuiContext::cursorEntered(bool entered) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddFocusEvent(entered);
    }

    bool SlabImGuiContext::notifyMouseButton(MouseButton button, KeyState state, ModKeys modKeys) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl,  modKeys.Mod_Ctrl == Press);
        io.AddKeyEvent(ImGuiMod_Shift, modKeys.Mod_Shift == Press);
        io.AddKeyEvent(ImGuiMod_Alt,   modKeys.Mod_Alt == Press);
        io.AddKeyEvent(ImGuiMod_Super, modKeys.Mod_Super == Press);

        io.AddMouseButtonEvent(button, state==Press);

        return io.WantCaptureMouse;
    }

    bool SlabImGuiContext::notifyMouseMotion(int x, int y, int dx, int dy) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddMousePosEvent((float)x, (float)y);

        return io.WantCaptureMouse;
    }

    bool SlabImGuiContext::notifyMouseWheel(double dx, double dy) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddMouseWheelEvent((float)dx, (float)dy);

        return io.WantCaptureMouse;
    }

    bool SlabImGuiContext::notifySystemWindowReshape(int w, int h) {
        ImGuiIO& io = ImGui::GetIO();

        io.DisplaySize = ImVec2((float)w, (float)h);

        return false;
    }

    bool SlabImGuiContext::notifyRender() {
        NewFrame();

        Render();
        return true;
    }

} // Slab::Graphics