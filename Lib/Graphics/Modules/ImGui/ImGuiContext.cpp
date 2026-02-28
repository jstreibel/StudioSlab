//
// Created by joao on 11/3/24.
//

#include "ImGuiContext.h"

#include <utility>

#include "Core/Tools/Log.h"

#include "ImGuiModule.h"
#include "ImGuiColorAndStyles.h"
#include "StudioSlab.h"

#include "Graphics/SlabGraphics.h"
#include "Graphics/Window/WindowStyles.h"

#include "Slab-ImGui-Interop.h"

namespace Slab::Graphics {
    #define FONT_SIZE_PIXELS Slab::Graphics::WindowStyle::font_size

    constexpr auto SHOW_DEAR_IMGUI_DEBUG_METRICS = false;

    FImGuiContext::FImGuiContext(FImplementationCallSet calls)
    : ImplementationCalls(std::move(calls)) {
        r_Context = ImGui::CreateContext();

        ImGui::SetCurrentContext(r_Context);

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable docking controls

        // ImGui::GetStyle().ScaleAllSizes(1.25);
        ImGui::GetIO().FontGlobalScale = 1;

        ImplementationCalls.Init(ImplementationCalls);
        BuildImGuiThemeFontAtlas(WindowStyle::font_size);

        Core::FLog::Info() << "Created ImGui context." << Core::FLog::Flush;
    }

    // SlabImGuiContext::SlabImGuiContext() {
    //     context = ImGui::CreateContext();
    //     ImGui::SetCurrentContext(context);
    // }

    //SlabImGuiContext::~SlabImGuiContext() {
    //    // ImGui::SetCurrentContext(context);
    //    ImGui::DestroyContext(context);
    //}

    void FImGuiContext::NewFrame() {
        ImGui::SetCurrentContext(r_Context);

        ImplementationCalls.NewFrame();
        ImGui::NewFrame();
    }

    void FImGuiContext::Render() {
        FlushDrawCalls();

        ImGui::Render();

        ImplementationCalls.Draw();
    }

    void FImGuiContext::SetupOptionalMenuItems()
    {
        FImGuiModule::SetupOptionalMenuItems(*this);
    }

    void FImGuiContext::Bind() {
        ImGui::SetCurrentContext(r_Context);
    }

    DevFloat FImGuiContext::GetFontSize() const {
        (void)this; // get rid of the annoying "this method can be made static" warning.

        return FONT_SIZE_PIXELS;
    }

    bool FImGuiContext::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl,  modKeys.Mod_Ctrl == Press);
        io.AddKeyEvent(ImGuiMod_Shift, modKeys.Mod_Shift == Press);
        io.AddKeyEvent(ImGuiMod_Alt,   modKeys.Mod_Alt == Press);
        io.AddKeyEvent(ImGuiMod_Super, modKeys.Mod_Super == Press);

        io.AddKeyEvent(SlabToImGuiTranslate(key), state == Press);

        return io.WantCaptureKeyboard;
    }

    bool FImGuiContext::NotifyCharacter(UInt codepoint) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddInputCharacter(codepoint);

        return io.WantTextInput;
    }

    void FImGuiContext::CursorEntered(bool entered) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddFocusEvent(entered);
    }

    bool FImGuiContext::NotifyMouseButton(EMouseButton button, EKeyState state, EModKeys modKeys) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGuiMod_Ctrl,  modKeys.Mod_Ctrl == Press);
        io.AddKeyEvent(ImGuiMod_Shift, modKeys.Mod_Shift == Press);
        io.AddKeyEvent(ImGuiMod_Alt,   modKeys.Mod_Alt == Press);
        io.AddKeyEvent(ImGuiMod_Super, modKeys.Mod_Super == Press);

        io.AddMouseButtonEvent(button, state==Press);

        return io.WantCaptureMouse;
    }

    bool FImGuiContext::NotifyMouseMotion(int x, int y, int dx, int dy) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddMousePosEvent((float)x, (float)y);

        return io.WantCaptureMouse;
    }

    bool FImGuiContext::NotifyMouseWheel(double dx, double dy) {
        Bind();

        ImGuiIO& io = ImGui::GetIO();

        io.AddMouseWheelEvent((float)dx, (float)dy);

        return io.WantCaptureMouse;
    }

    bool FImGuiContext::NotifySystemWindowReshape(int w, int h) {
        ImGuiIO& io = ImGui::GetIO();

        io.DisplaySize = ImVec2((float)w, (float)h);

        return false;
    }

    bool FImGuiContext::NotifyRender(const FPlatformWindow &Window) {
        if constexpr (SHOW_DEAR_IMGUI_DEBUG_METRICS) {
            AddMainMenuItem(MainMenuItem{
                MainMenuLocation{"System"},
                {
                    MainMenuLeafEntry{"Show Dear ImGui debug metrics", "Alt+m", false},
                },
                [&Window, this](const Str& Item)
                {
                    if (Item == "Close") const_cast<FPlatformWindow*>(&Window)->SignalClose();
                    else if (Item == "Show Dear ImGui debug metrics") bShowMetricsWindow = !bShowMetricsWindow;
                }
            });

            if (bShowMetricsWindow) AddDrawCall([this]
            {
                ImGui::ShowMetricsWindow(&bShowMetricsWindow);
            });
        }

        if (!bManualRender)
        {
            NewFrame();
            Render();
        }
        return true;
    }

    void AddItem(const int CurrentDepth, const MainMenuItem& Item)
    {
        fix& Location = Item.Location;
        fix MaxDepth = Location.size();

        if (ImGui::BeginMenu(Location[CurrentDepth].c_str())) {
            if (CurrentDepth < MaxDepth-1)
                AddItem(CurrentDepth+1, Item);
            else {
                const auto Action = Item.Action;
                for(const auto & [Label, Shortcut, Selected, Enabled] : Item.SubItems) {
                    if (Label == MainMenuSeparator)
                    {
                        ImGui::Separator();
                        continue;
                    }
                    if (ImGui::MenuItem(Label.c_str(), Shortcut.c_str(), Selected, Enabled))
                        Action(Label);
                }
            }

            ImGui::EndMenu();
        }
    };

    void FImGuiContext::AddMainMenuItem(MainMenuItem item) {
        AddDrawCall([item](){
            if(ImGui::BeginMainMenuBar()) {
                AddItem(0, item);

                ImGui::EndMainMenuBar();
            }
        });
    }

    void *FImGuiContext::GetContextPointer() {
        return r_Context;
    }

} // Slab::Graphics
