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

    using Context = TPointer<FImGuiContext>;

    class FImGuiModule : public FGUIModule {

    public:

        TPointer<FGUIContext> CreateContext(FOwnerPlatformWindow) override;

        static FImGuiModule* BuildModule();

        static void SetupOptionalMenuItems(FImGuiContext&);

    protected:
        Vector<Context> Contexts;

        bool bShowDemos = false;

        explicit FImGuiModule(FImplementationCallSet);

        ~FImGuiModule() override = default;

    private:
        FImplementationCallSet ImplCallSet;

    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULE_H
