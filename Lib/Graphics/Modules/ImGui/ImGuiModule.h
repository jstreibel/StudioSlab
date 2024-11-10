//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULE_H
#define STUDIOSLAB_IMGUIMODULE_H

#include "Core/Backend/Implementations.h"
#include "Core/Backend/Modules/Module.h"

#include "Graphics/Modules/GraphicsModule.h"

#include "3rdParty/ImGui.h"
#include "ImGuiContext.h"

namespace Slab::Graphics {

    using Context = Pointer<SlabImGuiContext>;

    class ImGuiModule : public GraphicsModule {
        Context m_MainContext = nullptr;
        CallSet call_set;

    protected:
        Vector<Context> contexts;

        bool showDemos = false;

        explicit ImGuiModule(CallSet);
        ~ImGuiModule() override;

    public:

        Pointer<SlabImGuiContext> createContext();

        static ImGuiModule* BuildModule();

        Context GetMainContext();

        void Update() override;
    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULE_H
