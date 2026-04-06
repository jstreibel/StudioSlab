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

    FGraphicBackend::FGraphicBackend(const Str &name)
    : FBackend(name) {}

    FGraphicBackend::~FGraphicBackend() = default;


    void FGraphicBackend::AddGraphicsModule(const TVolatile<FGraphicsModule>& module) { GraphicModules.emplace_back(module); }

    const Vector<TVolatile<FGraphicsModule>>& FGraphicBackend::GetGraphicsModules() { return GraphicModules; }

    bool FGraphicBackend::IsHeadless() const { return false; }

    TPointer<FPlatformWindow> FGraphicBackend::NewSystemWindow(const Str&title) {
        auto win = this->CreatePlatformWindow(title);

        SystemWindows.emplace_back(win);

        return win;
    }

    void FGraphicBackend::UnloadAllModules() { GraphicModules.clear(); }

    void FGraphicBackend::ClearModules() { GraphicModules.clear(); }

    void FGraphicBackend::Terminate() { UnloadAllModules(); }

    void FGraphicBackend::NotifyModuleLoaded(const TPointer<Core::FSlabModule> &module) {
        if(module->bRequiresGraphicsBackend) {
            auto GraphicModule = DynamicPointerCast<FGraphicsModule>(module);

            AddGraphicsModule(GraphicModule);
        }
    }

    TPointer<FPlatformWindow> FGraphicBackend::GetMainSystemWindow() {
        if(SystemWindows.empty()) return NewSystemWindow("Main window");

        return SystemWindows.front();
    }

    void FGraphicBackend::SetupGUIForPlatformWindow(const FPlatformWindow *PlatformWindow_RawPtr) const {
        if(PlatformWindow_RawPtr == nullptr)
        {
            Core::FLog::Error("While trying to setup a GUI context for nullptr SystemWindow.");
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

        Core::FLog::Error("Failed to setup GUI for platform window; window not found in backend data.");
    }
}
