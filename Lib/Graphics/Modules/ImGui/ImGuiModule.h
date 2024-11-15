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

    using Context = Pointer<SlabImGuiContext>;

    class ImGuiModule : public GUIModule {
        // Context m_MainContext = nullptr;
        CallSet call_set;

    protected:
        Vector<Context> contexts;

        bool showDemos = false;

        explicit ImGuiModule(CallSet);

        ~ImGuiModule() override = default;

    public:

        Pointer<GUIContext> createContext(ParentSystemWindow) override;

        static ImGuiModule* BuildModule();
    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULE_H
