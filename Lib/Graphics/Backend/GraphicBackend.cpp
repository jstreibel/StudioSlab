//
// Created by joao on 08/07/23.
//

#include "GraphicBackend.h"

#include <utility>
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"
#include "StudioSlab.h"
#include "Core/Tools/Log.h"
#include "Graphics/Modules/GUIModule/GUIModule.h"

namespace Slab::Graphics {

    GraphicBackend::GraphicBackend(const Str &name)
    : FBackend(name) {}

    GraphicBackend::~GraphicBackend() = default;


    void GraphicBackend::AddGraphicsModule(const Volatile<GraphicsModule> &module) {
        graphicModules.emplace_back(module);
    }

    const Vector<Volatile<GraphicsModule>> &GraphicBackend::GetGraphicsModules() {
        return graphicModules;
    }

    bool GraphicBackend::IsHeadless() const { return false; }

    Pointer<SystemWindow> GraphicBackend::NewSystemWindow(const Str&title) {
        auto win = this->CreateSystemWindow(title);

        SystemWindows.emplace_back(win);

        return win;
    }

    void GraphicBackend::UnloadAllModules() {
        graphicModules.clear();
    }


    void GraphicBackend::ClearModules() {
        graphicModules.clear();
    }

    void GraphicBackend::Terminate() {
        UnloadAllModules();
    }

    void GraphicBackend::NotifyModuleLoaded(const Pointer<Core::SlabModule> &module) {
        if(module->bRequiresGraphicsBackend) {
            auto graphic_module = DynamicPointerCast<GraphicsModule>(module);

            AddGraphicsModule(graphic_module);
        }
    }

    Pointer<SystemWindow> GraphicBackend::GetMainSystemWindow() {
        if(SystemWindows.empty()) return NewSystemWindow("Main window");

        return SystemWindows.front();
    }

    void GraphicBackend::SetupGUI(const SystemWindow *SysWin_RawPtr) const {
        if(SysWin_RawPtr == nullptr)
        {
            Core::Log::Error("While trying to setup a GUI context for nullptr SystemWindow.");
            return;
        }

        // If GUI is already setup for this window, then nothing to be done.
        if(SysWin_RawPtr->GuiContext != nullptr) return;

        for(const auto& SystemWindow : SystemWindows) {
            // SystemWindow must be registered in the Backend:
            if(SystemWindow.get() == SysWin_RawPtr) {
                auto &GuiModule = Slab::GetModule<FGUIModule>("GUI");

                SystemWindow->GuiContext = GuiModule.CreateContext(SystemWindow.get());
                SystemWindow->AddEventListener(SystemWindow->GuiContext);

                return;
            }
        }

        Core::Log::Error("Failed to setup GUI for platform window; window not found in backend data.");
    }
}