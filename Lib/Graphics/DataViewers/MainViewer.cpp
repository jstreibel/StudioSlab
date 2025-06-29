//
// Created by joao on 7/9/24.
//

#include "MainViewer.h"
#include "StudioSlab.h"
#include "Math/MathModule.h"
#include "Graphics/Modules/ImGui/ImGuiModule.h"

#include <utility>

namespace Slab::Graphics {

    MainViewer::MainViewer(Pointer<Math::R2toR::FNumericFunction> baseFunction)
    : Graphics::WindowRow()
    , base_function(std::move(baseFunction)) {

        auto font_size = gui_window->GetGUIContext()->getFontSize();

        auto gui_size = 22*font_size;

        addWindow(gui_window, Right, (float)gui_size);
        addResponder(gui_window);

        SetMinimumHeight((Resolution)(2.2*gui_size));
        SetMinimumWidth((Resolution)(4*gui_size));

        arrangeWindows();
    }

    void MainViewer::addViewer(const Pointer<Viewer>& viewer) {
        viewers.emplace_back(viewer);
        if(base_function!= nullptr) viewer->SetFunction(base_function);

        if(viewers.size()==1)
            setCurrentViewer(0);

    }

    void MainViewer::Draw() {
        auto gui_context = parent_system_window->getGUIContext().lock();

        if(gui_context == nullptr) return;

        gui_context->AddDrawCall(
                [this]() {
                    if (ImGui::BeginMainMenuBar()) {
                        if (ImGui::BeginMenu("Viewers")) {
                            Index i = 0;
                            for (auto &viewer: viewers) {
                                auto name = viewer->GetName();

                                fix is_current = getCurrentViewer() == viewer;
                                if (ImGui::MenuItem(name.c_str(), nullptr, is_current))
                                    setCurrentViewer(i);

                                ++i;
                            }

                            ImGui::EndMenu();
                        }

                        if (ImGui::BeginMenu("Datasets")) {
                            auto &mathModule = dynamic_cast<Math::MathModule &>(Slab::GetModule("Math"));
                            auto entries = mathModule.GetDataEntries();

                            if (entries.empty()) {
                                ImGui::MenuItem("No data available", nullptr, false, false);
                            } else {
                                for (auto &name: entries) {
                                    if (ImGui::MenuItem(name.name.c_str(), nullptr, false, false)) {
                                    }
                                }
                            }
                            ImGui::EndMenu();
                        }

                        ImGui::EndMainMenuBar();
                    }
                }
        );

        WindowRow::Draw();
    }

    bool MainViewer::NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        if(state==Press && key >= KeyMap::Key_1 && key <= KeyMap::Key_9)
            if(setCurrentViewer(key - KeyMap::Key_1)) return true;

        return WindowRow::NotifyKeyboard(key, state, modKeys);
    }

    bool MainViewer::setCurrentViewer(Index i) {
        if(i>viewers.size()-1) return false;

        auto old_viewer = current_viewer;
        auto new_viewer = viewers[i];

        if (new_viewer == old_viewer) return false;

        removeWindow(current_viewer);

        current_viewer = new_viewer;

        addWindow(current_viewer, Right, -1);

        arrangeWindows();
        current_viewer->NotifyBecameVisible();
        if (old_viewer != nullptr) old_viewer->notifyBecameInvisible();

        return true;
    }

    void MainViewer::setFunction(Pointer<Math::R2toR::FNumericFunction> function) {
        base_function = std::move(function);

        for(auto &viewer : viewers) viewer->SetFunction(base_function);
    }

    auto MainViewer::getGUIWindow() -> Pointer<FGUIWindow> {
        return gui_window;
    }

    auto MainViewer::getCurrentViewer() const -> Pointer<const Viewer> {
        return current_viewer;
    }

    auto MainViewer::getCurrentViewer() -> Pointer<Viewer> {
        return current_viewer;
    }


} // Studios::Fields::Viewers