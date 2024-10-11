//
// Created by joao on 7/9/24.
//

#include "MainViewer.h"

#include <utility>

namespace Slab::Graphics {

    MainViewer::MainViewer(Pointer<Math::R2toR::NumericFunction> baseFunction)
    : Graphics::WindowRow(HasMainMenu)
    , base_function(std::move(baseFunction)) {

        addWindow(gui_window, Right, 0.175);

        arrangeWindows();
    }

    void MainViewer::addViewer(const Pointer<Viewer>& viewer) {
        viewers.emplace_back(viewer);
        if(base_function!= nullptr) viewer->setFunction(base_function);

        if(viewers.size()==1)
            setCurrentViewer(0);

    }

    void MainViewer::draw() {
        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("Viewers")){
                Index i=0;
                for (auto &viewer: viewers) {
                    auto name = viewer->getName();

                    fix is_current = getCurrentViewer() == viewer;
                    if (ImGui::MenuItem(name.c_str(), nullptr, is_current))
                        setCurrentViewer(i);

                    ++i;
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        WindowRow::draw();
    }

    bool MainViewer::notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) {
        if(key >= Slab::Core::Key_1 && key <= Slab::Core::Key_9)
            if(setCurrentViewer(key - Slab::Core::Key_1)) return true;

        return WindowRow::notifyKeyboard(key, state, modKeys);
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

        new_viewer->notifyBecameVisible();
        if (old_viewer != nullptr) old_viewer->notifyBecameInvisible();

        return true;
    }

    void MainViewer::setFunction(Pointer<Math::R2toR::NumericFunction> function) {
        base_function = std::move(function);

        for(auto &viewer : viewers) viewer->setFunction(base_function);
    }

    auto MainViewer::getGUIWindow() -> Pointer<GUIWindow> {
        return gui_window;
    }

    auto MainViewer::getCurrentViewer() const -> Pointer<const Viewer> {
        return current_viewer;
    }

    auto MainViewer::getCurrentViewer() -> Pointer<Viewer> {
        return current_viewer;
    }


} // Studios::Fields::Viewers