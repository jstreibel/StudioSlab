//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULE_H
#define STUDIOSLAB_IMGUIMODULE_H

#include "Core/Backend/Implementations.h"

#include "3rdParty/ImGui.h"
#include "ImGuiContext.h"
#include "Graphics/Modules/GUIModule/GUIModule.h"

namespace Slab::Graphics {

    using Context = Pointer<FImGuiContext>;

    class FImGuiModule : public FGUIModule {
        // Context m_MainContext = nullptr;
        FCallSet CallSet;

    protected:
        Vector<Context> Contexts;

        bool bShowDemos = false;

        explicit FImGuiModule(FCallSet);

        ~FImGuiModule() override = default;

    public:

        Pointer<GUIContext> CreateContext(FOwnerPlatformWindow) override;

        static FImGuiModule* BuildModule();
    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULE_H
