//
// Created by joao on 08/07/23.
//

#include "GraphicBackend.h"

#include <utility>
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "StudioSlab.h"
#include "Graphics/Modules/GUIModule/GUIModule.h"

namespace Slab::Graphics {

    GraphicBackend::GraphicBackend(const Str &name)
    : FBackend(name) {}

    GraphicBackend::~GraphicBackend() = default;


    void GraphicBackend::addGraphicsModule(const Volatile<GraphicsModule> &module) {
        graphicModules.emplace_back(module);
    }

    const Vector<Volatile<GraphicsModule>> &GraphicBackend::getGraphicsModules() {
        return graphicModules;
    }

    bool GraphicBackend::IsHeadless() const { return false; }

    Pointer<SystemWindow> GraphicBackend::NewSystemWindow(const Str&title) {
        auto win = this->CreateSystemWindow(title);

        system_windows.emplace_back(win);

        return win;
    }

    void GraphicBackend::unloadAllModules() {
        graphicModules.clear();
    }


    void GraphicBackend::clearModules() {
        graphicModules.clear();
    }

    void GraphicBackend::Terminate() {
        unloadAllModules();
    }

    void GraphicBackend::notifyModuleLoaded(const Pointer<Core::SlabModule> &module) {
        if(module->bRequiresGraphicsBackend) {
            auto graphic_module = DynamicPointerCast<GraphicsModule>(module);

            addGraphicsModule(graphic_module);
        }
    }

    Pointer<SystemWindow> GraphicBackend::GetMainSystemWindow() {
        if(system_windows.empty()) return NewSystemWindow("Main window");

        return system_windows.front();
    }

    void GraphicBackend::SetupGUI(const SystemWindow *sys_win) const {
        if(sys_win->guiContext != nullptr) return;

        for(const auto& win : system_windows) {
            if(win.get() == sys_win) {
                auto &guiModule = Slab::GetModule<GUIModule>("GUI");

                win->guiContext = guiModule.CreateContext(win.get());
                win->AddEventListener(win->guiContext);

                return;
            }
        }

        throw Exception("Failed to setup GUI for platform window: window not found in backend data.");
    }
}