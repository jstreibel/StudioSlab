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

    struct FMenuNode {
        Str Label;
        Vector<FMenuNode> Children;
        Vector<std::pair<MainMenuLeafEntry, MainMenuAction>> Entries;
    };

    auto FindOrAddChild(FMenuNode &parent, const Str &label) -> FMenuNode* {
        for (auto &child : parent.Children) {
            if (child.Label == label) {
                return &child;
            }
        }
        parent.Children.push_back(FMenuNode{label});
        return &parent.Children.back();
    }

    auto BuildMenuTree(const Vector<MainMenuItem> &items) -> FMenuNode {
        FMenuNode root{"__root__"};
        for (const auto &item : items) {
            if (item.Location.empty()) continue;

            auto *node = &root;
            for (const auto &locationLevel : item.Location) {
                node = FindOrAddChild(*node, locationLevel);
            }

            for (const auto &subItem : item.SubItems) {
                node->Entries.emplace_back(subItem, item.Action);
            }
        }
        return root;
    }

    void DrawMenuNode(const FMenuNode &node) {
        for (const auto &child : node.Children) {
            if (!ImGui::BeginMenu(child.Label.c_str())) continue;

            DrawMenuNode(child);
            if (!child.Children.empty() && !child.Entries.empty()) {
                ImGui::Separator();
            }

            for (const auto &[entry, action] : child.Entries) {
                if (entry.Label == MainMenuSeparator) {
                    ImGui::Separator();
                    continue;
                }
                if (ImGui::MenuItem(
                        entry.Label.c_str(),
                        entry.shortcut.c_str(),
                        entry.selected,
                        entry.enabled)) {
                    action(entry.Label);
                }
            }

            ImGui::EndMenu();
        }
    }

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
        PendingMainMenuItems.clear();
    }

    void FImGuiContext::Render() {
        if (MainMenuPresentation == EMainMenuPresentation::MainMenuBar) {
            DrawMainMenuBar();
        }
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

    void FImGuiContext::AddMainMenuItem(MainMenuItem item) {
        PendingMainMenuItems.emplace_back(std::move(item));
    }

    void FImGuiContext::DrawMainMenuBar() {
        if (PendingMainMenuItems.empty()) return;
        if (!ImGui::BeginMainMenuBar()) return;

        const auto menuRoot = BuildMenuTree(PendingMainMenuItems);
        DrawMenuNode(menuRoot);

        ImGui::EndMainMenuBar();
    }

    void FImGuiContext::SetMainMenuPresentation(const EMainMenuPresentation presentation) {
        MainMenuPresentation = presentation;
    }

    auto FImGuiContext::GetMainMenuPresentation() const -> EMainMenuPresentation {
        return MainMenuPresentation;
    }

    auto FImGuiContext::DrawMainMenuLauncher(const char *id, const ImVec2 &size) -> bool {
        const auto launcherSize = ImVec2(
            (size.x > 0.0f) ? size.x : (ImGui::GetFrameHeight() + 8.0f),
            (size.y > 0.0f) ? size.y : ImGui::GetFrameHeight());

        ImGui::PushID(id);
        const bool clicked = ImGui::Button("##MainMenuLauncher", launcherSize);

        const auto rectMin = ImGui::GetItemRectMin();
        const auto rectMax = ImGui::GetItemRectMax();
        const auto width = rectMax.x - rectMin.x;
        const auto height = rectMax.y - rectMin.y;
        const auto left = rectMin.x + 0.27f * width;
        const auto right = rectMax.x - 0.27f * width;
        const auto y0 = rectMin.y + 0.32f * height;
        const auto y1 = rectMin.y + 0.50f * height;
        const auto y2 = rectMin.y + 0.68f * height;
        const auto color = ImGui::GetColorU32(ImGuiCol_Text);
        const auto thickness = 2.0f;
        auto *drawList = ImGui::GetWindowDrawList();
        drawList->AddLine(ImVec2(left, y0), ImVec2(right, y0), color, thickness);
        drawList->AddLine(ImVec2(left, y1), ImVec2(right, y1), color, thickness);
        drawList->AddLine(ImVec2(left, y2), ImVec2(right, y2), color, thickness);

        if (clicked) {
            ImGui::OpenPopup("##MainMenuLauncherPopup");
        }

        if (ImGui::BeginPopup("##MainMenuLauncherPopup")) {
            if (PendingMainMenuItems.empty()) {
                ImGui::TextDisabled("No menu entries");
            } else {
                const auto menuRoot = BuildMenuTree(PendingMainMenuItems);
                DrawMenuNode(menuRoot);
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();
        return clicked;
    }

    void *FImGuiContext::GetContextPointer() {
        return r_Context;
    }

} // Slab::Graphics
