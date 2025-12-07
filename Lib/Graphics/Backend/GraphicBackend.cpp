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

#include "PlatformWindow.h"

namespace Slab::Graphics {

    GraphicBackend::GraphicBackend(const Str &name)
    : FBackend(name) {}

    GraphicBackend::~GraphicBackend() = default;


    void GraphicBackend::AddGraphicsModule(const TVolatile<GraphicsModule> &module) { GraphicModules.emplace_back(module); }

    const Vector<TVolatile<GraphicsModule>> &GraphicBackend::GetGraphicsModules() { return GraphicModules; }

    bool GraphicBackend::IsHeadless() const { return false; }

    TPointer<FPlatformWindow> GraphicBackend::NewSystemWindow(const Str&title) {
        auto win = this->CreatePlatformWindow(title);

        SystemWindows.emplace_back(win);

        return win;
    }

    void GraphicBackend::UnloadAllModules() { GraphicModules.clear(); }

    void GraphicBackend::ClearModules() { GraphicModules.clear(); }

    void GraphicBackend::Terminate() { UnloadAllModules(); }

    void GraphicBackend::NotifyModuleLoaded(const TPointer<Core::SlabModule> &module) {
        if(module->bRequiresGraphicsBackend) {
            auto GraphicModule = DynamicPointerCast<GraphicsModule>(module);

            AddGraphicsModule(GraphicModule);
        }
    }

    TPointer<FPlatformWindow> GraphicBackend::GetMainSystemWindow() {
        if(SystemWindows.empty()) return NewSystemWindow("Main window");

        return SystemWindows.front();
    }

    void GraphicBackend::SetupGUIForPlatformWindow(const FPlatformWindow *PlatformWindow_RawPtr) const {
        if(PlatformWindow_RawPtr == nullptr)
        {
            Core::Log::Error("While trying to setup a GUI context for nullptr SystemWindow.");
            return;
        }

        // If GUI is already setup for this window, then nothing to be done.
        if(PlatformWindow_RawPtr->GuiContext != nullptr) return;

        for(const auto& PlatformWindow_It : SystemWindows) {
            // PlatformWindow must be registered in the Backend:
            if(PlatformWindow_It.get() == PlatformWindow_RawPtr) {
                auto &GuiModule = Slab::GetModule<FGUIModule>("GUI");

                PlatformWindow_It->GuiContext = GuiModule.CreateContext(PlatformWindow_It);

                return;
            }
        }

        Core::Log::Error("Failed to setup GUI for platform window; window not found in backend data.");
    }
}